#include "FlatMesher/FlatMesh.h"
#include "FlatMesher/FloorPlan.h"
#include "FlatMesher/Line2.h"
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
  }

  merge(walls, boundaries, ceiling);
}

Mesh FlatMesh::createWall(const Point2& a, const Point2& b) const {
  Mesh wall;

  double delta = m_plan->getTriangleSize();

  // z represents the height, and xy is the plane where the two points are
  size_t nodes_z = lround(m_plan->getHeight() / delta);
  size_t nodes_xy = lround(a.distance(b) / delta);

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

  Point2 rectOffset = box.getLowerLeft();
  size_t height = lround(box.getHeight() / delta);
  size_t width = lround(box.getWidth() / delta);

  // This will be used to save the index of the node located at the
  // last processed row. If its value is numeric_limits::max, it means that
  // it isn't part of the mesh
  std::vector<size_t> row_idx(width + 1, std::numeric_limits<size_t>::max());

  // Fill the first row and create the first nodes, saving its indices
  for (size_t ix = 0; ix <= width; ++ix) {
    Point2 p(rectOffset.getX() + (ix * delta), rectOffset.getY());

    bool boundary = m_plan->pointInBoundary(p);
    bool inside = boundary || m_plan->pointInside(p);

    if (inside) {
      row_idx[ix] = ceiling.addNode(Point3(p, m_plan->getHeight()));
      if (boundary)
        boundary_nodes.push_back(row_idx[ix]);
    }
  }

  // We iterate through the bounding box analyzing each group of 4 points
  // in order to create the triangular mesh. The middle point is used to know
  // if the square belongs to the ceiling.
  // d --- c
  // |  m  |
  // a --- b
  for (size_t iy = 1; iy <= height; ++iy) {
    double y = rectOffset.getY() + (iy * delta);

    std::vector<size_t> current_row_idx(width + 1, std::numeric_limits<size_t>::max());

    // We compute the first iteration separately, so that the nodes in the
    // first column can be processed (base case)
    size_t a_idx = row_idx[0];
    size_t b_idx = row_idx[1];
    size_t c_idx = std::numeric_limits<size_t>::max();
    size_t d_idx = std::numeric_limits<size_t>::max();

    // Coordinates of the points we will create
    Point2 c(rectOffset.getX() + delta, y);
    Point2 d(rectOffset.getX(), y);
    Point2 m(rectOffset.getX() + (delta / 2.0), y - (delta / 2.0));

    // Check if the two new points are part of the boundary
    bool c_bo = m_plan->pointInBoundary(c);
    bool d_bo = m_plan->pointInBoundary(d);

    // For each point: Is it inside the polygon?
    bool a_in = a_idx != std::numeric_limits<size_t>::max();
    bool b_in = b_idx != std::numeric_limits<size_t>::max();
    bool c_in = c_bo || m_plan->pointInside(c);
    bool d_in = d_bo || m_plan->pointInside(d);
    bool m_in = m_plan->pointInBoundary(m) || m_plan->pointInside(m);

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
    submesh(a_idx, b_idx, c_idx, d_idx, a_in, b_in, c_in, d_in, m_in, ceiling);

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
      double x = rectOffset.getX() + (ix * delta);

      a_idx = b_idx;
      d_idx = c_idx;
      b_idx = row_idx[ix];

      d = c;
      c = Point2(x, y);
      m = Point2(x - (delta / 2.0), y - (delta / 2.0));

      c_bo = m_plan->pointInBoundary(c);

      a_in = b_in;
      d_in = c_in;
      b_in = b_idx != std::numeric_limits<size_t>::max();
      c_in = c_bo || m_plan->pointInside(c);
      m_in = m_plan->pointInBoundary(m) || m_plan->pointInside(m);

      if (c_in) {
        c_idx = ceiling.addNode(Point3(c, m_plan->getHeight()));
        if (c_bo)
          boundary_nodes.push_back(c_idx);
      }

      submesh(a_idx, b_idx, c_idx, d_idx, a_in, b_in, c_in, d_in, m_in, ceiling);
      current_row_idx[ix] = c_idx;
    }

    // We change the indices for the processing of the next row
    std::swap(row_idx, current_row_idx);
  }

  // Maybe return a list of indices of nodes that are part of the boundaries, so
  // that the search for the nodes doesn't take long
  return ceiling;
}

void FlatMesh::merge(const std::vector<Mesh>& walls, const std::vector<size_t>& boundaries, const Mesh& ceiling) {
  // We merge the walls by offsetting the indices and creating the sub-mesh
  // of each corner that we didn't create before
  std::vector<size_t> nodes_amount(walls.size());
  size_t acc_nodes = 0;

  size_t nodes_z = lround(m_plan->getHeight() / m_plan->getTriangleSize()) + 1;
  size_t total_nodes = lround(m_plan->boundaryLength() * nodes_z);

  for (size_t i = 0; i < walls.size(); ++i) {
    Mesh wall = walls[i];

    std::vector<Point3> nodes = wall.getNodes();
    std::vector<IndexTriangle> indices = wall.getMesh(acc_nodes);

    m_nodes.insert(m_nodes.end(), nodes.begin(), nodes.end());
    m_mesh.insert(m_mesh.end(), indices.begin(), indices.end());

    acc_nodes += nodes.size();
    nodes_amount[i] = nodes.size();

    for (size_t j = 0; j < nodes_z - 1; ++j) {
      size_t actual_idx = acc_nodes - nodes_z + j;

      IndexTriangle t1(actual_idx, (actual_idx + nodes_z) % total_nodes, (actual_idx + nodes_z + 1) % total_nodes);
      IndexTriangle t2(actual_idx, (actual_idx + nodes_z + 1) % total_nodes, actual_idx + 1);

      m_mesh.push_back(t1);
      m_mesh.push_back(t2);
    }
  }

  // Creation of the floor mesh from the ceiling mesh
  Mesh floor = ceiling;
  floor.move(0, 0, -m_plan->getHeight());
  floor.invert();

  std::vector<Point3> nodes = ceiling.getNodes();
  std::map<size_t, size_t> tr_floor;

  std::vector<Point2> plan_nodes = m_plan->getNodes();
  size_t plan_sz = plan_nodes.size();
  size_t boundary_sz = boundaries.size();

  acc_nodes = 0;

  // Walk over every segment and find the corresponding nodes in the ceiling
  // to fill the translation table
  for (size_t i = 0; i < plan_sz; ++i) {
    Point2 a = plan_nodes[i];
    Point2 b = plan_nodes[(i + 1) % plan_sz];

    Line2 ab(a, b);

    for (size_t j = 0; j < boundary_sz; ++j) {
      Point3 boundary_point = nodes[boundaries[j]];
      Point2 boundary_2d(boundary_point.getX(), boundary_point.getY());

      if (ab.contains(boundary_2d)) {
        // Find the index in the line where the "boundary_point" is
        // We know that nodes are ordered by columns from bottom to top, so
        // it's only needed the column index and the index where the current
        // wall starts to figure out the global index of the corresponding top
        // and bottom nodes (ceiling and floor)
        size_t wall_column_idx = lround(a.distance(boundary_2d) / m_plan->getTriangleSize());
        tr_floor[boundaries[j]] = (acc_nodes + wall_column_idx * nodes_z) % total_nodes;
      }
    }

    acc_nodes += nodes_amount[i];
  }

  // Get the nodes which are not part of the boundaries and add them to the mesh
  for (size_t i = 0; i < nodes.size(); ++i) {
    if (tr_floor.find(i) == tr_floor.end())
      m_nodes.push_back(nodes[i]);
  }

  nodes = floor.getNodes();
  for (size_t i = 0; i < nodes.size(); ++i) {
    if (tr_floor.find(i) == tr_floor.end())
      m_nodes.push_back(nodes[i]);
  }

  // Take the meshes of the ceiling and floor and change the indices to match
  // the new ones, by applying offsets and translations
  size_t ceil_offset = total_nodes;
  size_t floor_offset = total_nodes + nodes.size() - boundary_sz;

  std::vector<IndexTriangle> mesh_ceil = ceiling.getMesh(ceil_offset);
  std::vector<IndexTriangle> mesh_floor = floor.getMesh(floor_offset);

  #pragma omp parallel for schedule(guided) shared(mesh_ceil, mesh_floor)
  for (int i = 0; i < (int) mesh_ceil.size(); ++i) {
    processTriangle(boundaries, tr_floor, ceil_offset, nodes_z - 1, mesh_ceil[i]);
    processTriangle(boundaries, tr_floor, floor_offset, 0, mesh_floor[i]);
  }

  // Add all the new triangles to the flat's mesh
  m_mesh.insert(m_mesh.end(), mesh_ceil.begin(), mesh_ceil.end());
  m_mesh.insert(m_mesh.end(), mesh_floor.begin(), mesh_floor.end());
}

void FlatMesh::submesh(size_t a_idx, size_t b_idx, size_t c_idx, size_t d_idx,
                       bool a_in, bool b_in, bool c_in, bool d_in, bool m_in, Mesh& mesh) {
  // d --- c
  // |  m  |
  // a --- b
  if (m_in) {
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
}

void FlatMesh::processTriangle(const std::vector<size_t>& boundaries,
                               const std::map<size_t, size_t>& tr_floor, size_t input_offset,
                               size_t output_offset, IndexTriangle& triangle) {
  triangle.setI(processTriangleIndex(boundaries, tr_floor, input_offset, output_offset, triangle.getI()));
  triangle.setJ(processTriangleIndex(boundaries, tr_floor, input_offset, output_offset, triangle.getJ()));
  triangle.setK(processTriangleIndex(boundaries, tr_floor, input_offset, output_offset, triangle.getK()));
}

size_t FlatMesh::processTriangleIndex(const std::vector<size_t>& boundaries,
                                      const std::map<size_t, size_t>& tr_floor, size_t input_offset,
                                      size_t output_offset, size_t idx) {
  std::map<size_t, size_t>::const_iterator itm;

  // If the index has to be translated, we just use the translation table
  if ((itm = tr_floor.find(idx - input_offset)) != tr_floor.end())
    return itm->second + output_offset;

  // In case it does not have to be translated, we decrement the index as many
  // times as nodes with smaller indices are in the boundaries of the mesh
  // This is needed in order to keep the indices of the mesh correct after
  // deleting the boundary nodes
  else {
    size_t i;
    for (i = 0; i < boundaries.size(); ++i)
      if (boundaries[i] > idx - input_offset)
        break;

    return idx - i;
  }
}
