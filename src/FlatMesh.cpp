#include "FlatMesher/FlatMesh.h"
#include "FlatMesher/FloorPlan.h"
#include "FlatMesher/Point2.h"
#include "FlatMesher/Rectangle.h"
#include "FlatMesher/Utils.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace flat;

void FlatMesh::createFromPlan(const FloorPlan* plan) {
  if (m_plan != NULL) {
    m_nodes.clear();
    m_mesh.clear();
  }

  if (plan == NULL || !plan->valid())
    return;

  m_plan = plan;

  std::vector<Point2> plan_nodes = plan->getNodes();
  int plan_sz = plan_nodes.size();
  std::vector<Mesh> walls(plan_sz);

  Mesh ceiling;
  std::vector<size_t> boundaries;

  #pragma omp parallel shared(walls, ceiling, boundaries)
  {
    #pragma omp for schedule(dynamic) nowait
    for (int i = 1; i <= plan_sz; ++i) {
      Point2 a = plan_nodes[i - 1];
      Point2 b = plan_nodes[i % plan_sz];

      Mesh wall = createWall(a, b);

      // Thread-safe
      walls[i - 1] = wall;
    }

    // This can be executed before the for loop has finished
    #pragma omp single
    ceiling = createCeiling(plan->boundingBox(), boundaries);

    // This only executes once the previous block and the loop are finished
    #pragma omp single
    merge(walls, ceiling);
  }
}

Mesh FlatMesh::createWall(const Point2& a, const Point2& b) const {
  Mesh wall;

  double delta = m_plan->getTriangleSize();

  // z represents the height, and xy is the plane where the two points are
  size_t nodes_z = (size_t) (m_plan->getHeight() / delta);
  size_t nodes_xy = (size_t) (a.distance(b) / delta);

  // We use this to be able to iterate over the line that contains the two
  // points, even when it is not vertical or horizontal
  double delta_x = (b.getX() - a.getX()) / nodes_xy;
  double delta_y = (b.getY() - a.getY()) / nodes_xy;

  // Add the nodes in column order and from bottom to top
  // Leave out the last column, because it's created in the next wall
  // The edges between the last column and the second to last column are added by
  // the merge() method
  Point3 origin(a);
  for (size_t i = 0; i < nodes_xy; ++i)
    for (size_t j = 0; j <= nodes_z; ++j)
      wall.addNode(origin + Point3(i * delta_x, i * delta_y, j * delta));

  // Add the edges that compose the mesh
  // For this to always order the indices of the nodes in counter-clockwise order
  // the 2D points of the floor plan must be specified in that order also
  for (size_t i = 0; i < nodes_xy - 1; ++i) {
    for (size_t j = 0; j < nodes_z; ++j) {
      size_t actual_idx = (i * (nodes_z + 1)) + j;

      /* /|
        / |
       o__| */
      IndexTriangle t1(actual_idx, actual_idx + nodes_z + 1, actual_idx + nodes_z + 2);

      /* ____
         |  /
         | /
         o/  */
      IndexTriangle t2(actual_idx, actual_idx + nodes_z + 2, actual_idx + 1);

      wall.addTriangle(t1);
      wall.addTriangle(t2);
    }
  }

  return wall;
}

Mesh FlatMesh::createCeiling(const Rectangle& box, std::vector<size_t>& boundary_nodes) const {
  Mesh ceiling;

  double delta = m_plan->getTriangleSize();
  size_t height = (size_t) (box.getHeight() / delta);
  size_t width = (size_t) (box.getWidth() / delta);

  // This will be used to save the index of the node located at the
  // last processed row. If its value is numeric_limits::max, it means that
  // it isn't part of the mesh
  std::vector<size_t> row_idx(width + 1, std::numeric_limits<size_t>::max());

  // Fill the first row and create the first nodes, saving its indices
  for (size_t ix = 0; ix <= width; ++ix) {
    Point2 p(ix * delta, 0.0);

    bool boundary = m_plan->pointInBoundary(p);
    bool inside = boundary || m_plan->pointInside(p);

    if (inside) {
      row_idx[ix] = ceiling.addNode(Point3(p, m_plan->getHeight()));
      if (boundary)
        boundary_nodes.push_back(row_idx[ix]);
    }
  }

  // We iterate through the bounding box analyzing each group of 4 points
  // in order to create the triangular mesh
  // d --- c
  // |     |
  // a --- b
  for (size_t iy = 1; iy <= height; ++iy) {
    double y = iy * delta;

    std::vector<size_t> current_row_idx(width + 1, std::numeric_limits<size_t>::max());

    // We compute the first iteration separately, so that the nodes in the
    // first column can be processed (base case)
    size_t a_idx = row_idx[0];
    size_t b_idx = row_idx[1];
    size_t c_idx = std::numeric_limits<size_t>::max();
    size_t d_idx = std::numeric_limits<size_t>::max();

    // Coordinates of the points we will create
    Point2 c(delta, y);
    Point2 d(0, y);

    // Check if the two new points are part of the boundary
    bool c_bo = m_plan->pointInBoundary(c);
    bool d_bo = m_plan->pointInBoundary(d);

    // For each point: Is it inside the polygon?
    bool a_in = a_idx != std::numeric_limits<size_t>::max();
    bool b_in = b_idx != std::numeric_limits<size_t>::max();
    bool c_in = c_bo || m_plan->pointInside(c);
    bool d_in = d_bo || m_plan->pointInside(d);

    // Add the new two points if they are part of the mesh
    if (d_in) {
      d_idx = ceiling.addNode(Point3(d, m_plan->getHeight()));
      if (d_bo)
        boundary_nodes.push_back(d_idx);
    }

    if (c_in) {
      c_idx = ceiling.addNode(Point3(c, m_plan->getHeight()));
      if (c_bo)
        boundary_nodes.push_back(c_idx);
    }

    // Create the triangles
    submesh(a_idx, b_idx, c_idx, d_idx, a_in, b_in, c_in, d_in, ceiling);

    // Save indices for the next row
    current_row_idx[0] = d_idx;
    current_row_idx[1] = c_idx;

    // Processing of the rest of columns
    // We reuse the information of the previous column and the previous row
    // in order to avoid double calculations and duplicate indices, so in each
    // iteration we just have to check the node "c" of the square (top right
    // corner)
    // Other than that, the computation is pretty much the same that the
    // first iteration
    for (size_t ix = 2; ix <= width; ++ix) {
      double x = ix * delta;

      a_idx = b_idx;
      d_idx = c_idx;
      b_idx = row_idx[ix];

      d = c;
      c = Point2(x, y);

      c_bo = m_plan->pointInBoundary(c);

      a_in = b_in;
      d_in = c_in;
      b_in = b_idx != std::numeric_limits<size_t>::max();
      c_in = c_bo || m_plan->pointInside(c);

      if (c_in) {
        c_idx = ceiling.addNode(Point3(c, m_plan->getHeight()));
        if (c_bo)
          boundary_nodes.push_back(c_idx);
      }

      submesh(a_idx, b_idx, c_idx, d_idx, a_in, b_in, c_in, d_in, ceiling);
      current_row_idx[ix] = c_idx;
    }

    // We change the indices for the processing of the next row
    std::swap(row_idx, current_row_idx);
  }

  // Maybe return a list of indices of nodes that are part of the boundaries, so
  // that the search for the nodes doesn't take long
  return ceiling;
}

void FlatMesh::merge(const std::vector<Mesh>& walls, const Mesh& ceiling) {
  // We merge the walls by offsetting the indices and creating the sub-mesh
  // of each corner that we didn't create before
  size_t acc_nodes = 0;
  size_t nodes_z = (size_t) (m_plan->getHeight() / m_plan->getTriangleSize()) + 1;
  size_t total_nodes = (size_t) m_plan->boundaryLength() * nodes_z;

  double boundary = m_plan->boundaryLength();
  for (size_t i = 0; i < walls.size(); ++i) {
    Mesh wall = walls[i];

    std::vector<Point3> nodes = wall.getNodes();
    std::vector<IndexTriangle> indices = wall.getMesh(acc_nodes);

    m_nodes.insert(m_nodes.end(), nodes.begin(), nodes.end());
    m_mesh.insert(m_mesh.end(), indices.begin(), indices.end());

    acc_nodes += nodes.size();

    for (size_t j = 0; j < nodes_z - 1; ++j) {
      size_t actual_idx = acc_nodes - nodes_z + j;

      IndexTriangle t1(actual_idx, (actual_idx + nodes_z) % total_nodes, (actual_idx + nodes_z + 1) % total_nodes);
      IndexTriangle t2(actual_idx, (actual_idx + nodes_z + 1) % total_nodes, actual_idx + 1);

      m_mesh.push_back(t1);
      m_mesh.push_back(t2);
    }
  }

  // TODO Create the floor. Merge ceiling and floor with the rest of the mesh.
  // The indices coming from the ceiling have to be offset by "total_nodes"
  // Then it's needed to walk over every segment and find the corresponding
  // nodes in the ceiling to delete them and change the triangles where they appear
  // Maybe it's a good idea to create a translation table and then apply it
}

void FlatMesh::submesh(size_t a_idx, size_t b_idx, size_t c_idx, size_t d_idx,
                       bool a_in, bool b_in, bool c_in, bool d_in, Mesh& mesh) {
  // d --- c
  // |     |
  // a --- b
  if (a_in) {
    if (c_in) {
      if (b_in)
        mesh.addTriangle(IndexTriangle(a_idx, b_idx, c_idx));
      if (d_in)
        mesh.addTriangle(IndexTriangle(a_idx, c_idx, d_idx));
    }
    else if (b_in && d_in)
      mesh.addTriangle(IndexTriangle(a_idx, b_idx, d_idx));
  }
  else if (b_in && c_in && d_in)
    mesh.addTriangle(IndexTriangle(b_idx, c_idx, d_idx));
}

std::ostream& operator<<(std::ostream& os, const flat::FlatMesh& mesh) {
  std::vector<Point3> nodes = mesh.getNodes();
  std::vector<IndexTriangle> triangles = mesh.getMesh();

  os << "3\n3\n\n";

  os << nodes.size() << '\n';
  for (auto i = nodes.begin(); i != nodes.end(); ++i)
    os << *i << '\n';

  os << '\n' << triangles.size();
  for (auto i = triangles.begin(); i != triangles.end(); ++i)
    os << '\n' << *i;

  return os;
}

std::istream& operator>>(std::istream& is, flat::FlatMesh& mesh) {
  size_t sp_dim, nodes_el;

  is >> sp_dim;
  is >> nodes_el;
  if (sp_dim != 3 || nodes_el != 3) {
    is.clear(std::ios::failbit);
    return is;
  }

  size_t total_nodes;
  is >> total_nodes;

  std::vector<Point3> nodes(total_nodes);
  for (size_t i = 0; i < total_nodes; ++i)
    is >> nodes[i];

  size_t total_edges;
  is >> total_edges;

  std::vector<IndexTriangle> edges(total_edges, IndexTriangle(0, 0, 0));
  for (size_t i = 0; i < total_edges; ++i)
    is >> edges[i];

  mesh.setMesh(nodes, edges);

  return is;
}
