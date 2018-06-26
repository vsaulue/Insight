/*
 * This file is part of Insight.
 * Copyright (C) 2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
 *
 * Insight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Insight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Insight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GraphicObject.hpp"
#include "irrlicht_ptr.hpp"
#include "ShapeDrawer.hpp"
#include "units/IrrUnits.hpp"
#include "units/Scalar.hpp"
#include "units/SI.hpp"

/**
 * Converts a Bullet vector into an Irrlicht vector.
 * @param[in] vector Buller vector.
 * @return Irrlicht vector.
 */
static irr::core::vector3df btToIrrVector(const btVector3& vector) {
    return irr::core::vector3df(vector.x(), vector.y(), vector.z());
}

/**
 * Converts a Bullet quaternion to a vector of Euler angles (Irrlicht format).
 * @param[in] btQuat Bullet quaternion.
 * @return Vector of Irrlicht Euler angles.
 */
static irr::core::vector3df btQuaternionToEulerAngles(const btQuaternion& btQuat) {
    irr::core::quaternion irrQuat(btQuat.x(), btQuat.y(), btQuat.z(), btQuat.w());
    irr::core::vector3df result;
    irrQuat.toEuler(result);
    return irr::core::RADTODEG*result;
}

/**
 * Class converting the collision shape of a body into a 3d scene node.
 */
class IrrlichtDrawer : public ShapeDrawer {
public:
    /* This implementation represents the plane with a square. */
    void drawPlane(const btVector3& normal, btScalar offset) override {
        using namespace irr::scene;

        ISceneManager& scene = *rootNode.getSceneManager();
        irrlicht_ptr<SMeshBuffer> meshBuffer(new SMeshBuffer());
        irrlicht_ptr<SMesh> mesh(new SMesh());
        mesh->addMeshBuffer(meshBuffer.get());

        std::array<irr::video::S3DVertex, 4> vertices;
        getPlaneVertices(normal, offset, vertices);
        for (auto& vertice : vertices) {
            meshBuffer->Vertices.push_back(vertice);
        }
        static const std::array<irr::u16,6> faces = {0,2,1,1,2,3};
        for (auto& index : faces) {
            meshBuffer->Indices.push_back(index);
        }
        meshBuffer->recalculateBoundingBox();
        mesh->recalculateBoundingBox();

        scene.addMeshSceneNode(mesh.get(), &rootNode);
    }

    void drawSphere(const btVector3& center, btScalar radius) override {
        irr::scene::ISceneManager& scene = *rootNode.getSceneManager();
        irr::core::vector3df pos = btToIrrVector(center);
        scene.addSphereSceneNode(radius, 16, &rootNode, -1, pos);
    }

    void drawCylinder(const btTransform& transform, const btVector3& halfExtents) override {
        using vector3df = irr::core::vector3df;
        irr::scene::ISceneManager& scene = *rootNode.getSceneManager();
        static irrlicht_ptr<irr::scene::IMesh> cylinder(makeCylinderMesh());

        vector3df pos = btToIrrVector(transform.getOrigin());
        vector3df scale = btToIrrVector(halfExtents);
        vector3df rotation = btQuaternionToEulerAngles(transform.getRotation());
        scene.addMeshSceneNode(cylinder.get(), &rootNode, -1, pos, rotation, scale);
    }

    void drawCuboid(const btTransform& transform, const btVector3& halfExtents) override {
        irr::scene::ISceneManager& scene = *rootNode.getSceneManager();
        static irrlicht_ptr<irr::scene::IMesh> cuboid(makeCuboidMesh());
        irr::core::vector3df pos = btToIrrVector(transform.getOrigin());
        irr::core::vector3df scale = btToIrrVector(halfExtents);
        irr::core::vector3df rotation = btQuaternionToEulerAngles(transform.getRotation());
        scene.addMeshSceneNode(cuboid.get(), &rootNode, -1, pos, rotation, scale);
    }

    void drawMesh(const btTransform& transform, const ConvexMesh& physicsMesh) override {
        irr::scene::ISceneManager& scene = *rootNode.getSceneManager();
        irrlicht_ptr<irr::scene::IMesh> mesh(makeMesh(physicsMesh));
        irr::core::vector3df pos = btToIrrVector(transform.getOrigin());
        irr::core::vector3df rotation = btQuaternionToEulerAngles(transform.getRotation());
        scene.addMeshSceneNode(mesh.get(), &rootNode, -1, pos, rotation);
    }

    IrrlichtDrawer(irr::scene::ISceneNode& rootNode) : rootNode(rootNode) {

    }
private:
    /** Node that will hold the 3d representation of a collision shape. */
    irr::scene::ISceneNode& rootNode;

    /**
     * Generates a new cylinder mesh.
     *
     * Y-axis, centered on {0,0,0}, radius 1, length 2.
     *
     * @param[in] tesselation Number of quads used for the sides of the cylinder.
     * @return A mesh representing a cylinder.
     */
    static irrlicht_ptr<irr::scene::IMesh> makeCylinderMesh(irr::u32 tesselation = 16) {
        using namespace irr::scene;
        irrlicht_ptr<SMeshBuffer> meshBuffer(new SMeshBuffer());
        meshBuffer->getMaterial().NormalizeNormals = true;
        irrlicht_ptr<SMesh> result(new SMesh());
        result->addMeshBuffer(meshBuffer.get());
        irr::core::array<irr::video::S3DVertex>& vertices = meshBuffer->Vertices;
        irr::core::array<irr::u16>& indices = meshBuffer->Indices;

        // top & bottom circles.
        for (int y = -1; y<=1; y+=2) {
            irr::video::S3DVertex vertex(0,y,0, 0,y,0, irr::video::SColor(255,255,255,255), 0, 0);
            vertices.push_back(vertex);
            for (unsigned index = 0; index < tesselation; index++) {
                float angle = static_cast<float>(index)/tesselation*2*irr::core::PI;
                vertex.Pos.X = btCos(angle);
                vertex.Pos.Z = btSin(angle);
                vertices.push_back(vertex);
            }
        }
        for (unsigned index = 0; index < tesselation; index++) {
            indices.push_back(0);
            indices.push_back(1+index);
            indices.push_back(1+(index+1)%tesselation);
        }
        int indexOffset = tesselation+1;
        for (unsigned index = 0; index < tesselation; index++) {
            indices.push_back(indexOffset);
            indices.push_back(indexOffset+1+(index+1)%tesselation);
            indices.push_back(indexOffset+1+index);
        }

        // side quads.
        indexOffset = 2*tesselation+2;
        irr::video::S3DVertex vertex(0,0,0, 0,0,0, irr::video::SColor(255,255,255,255), 0, 0);
        for (unsigned index = 0; index < tesselation; index++) {
            float angle = static_cast<float>(index)/tesselation*2*irr::core::PI;
            vertex.Pos.X = btCos(angle);
            vertex.Normal.X = vertex.Pos.X;
            vertex.Pos.Z = btSin(angle);
            vertex.Normal.Z = vertex.Pos.Z;

            vertex.Pos.Y = 1;
            vertices.push_back(vertex);
            vertex.Pos.Y = -1;
            vertices.push_back(vertex);

            int a = indexOffset+2*index;
            int b = a+1;
            int c = indexOffset+(2*index+2)%(2*tesselation);
            int d = c+1;
            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(b);

            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(d);
        }

        return result;
    }

    /**
     * Generates the mesh of a cube of side 2.
     *
     * NO texture coordinates.
     *
     * @return The mesh of a cube.
     */
    static irrlicht_ptr<irr::scene::IMesh> makeCuboidMesh() {
        using namespace irr::scene;
        irrlicht_ptr<SMeshBuffer> meshBuffer(new SMeshBuffer());
        meshBuffer->getMaterial().NormalizeNormals = true;
        irrlicht_ptr<SMesh> result(new SMesh());
        result->addMeshBuffer(meshBuffer.get());
        static const std::array<irr::core::vector3df,8> VERTICES = {{
            {-1,-1,-1},
            {-1,-1, 1},
            { 1,-1, 1},
            { 1,-1,-1},
            {-1, 1,-1},
            {-1, 1, 1},
            { 1, 1, 1},
            { 1, 1,-1}
        }};
        static const std::array<std::pair<std::array<int,4>, irr::core::vector3df>,6> FACES = {{
            {{0,3,2,1}, {0,-1,0}},
            {{0,1,5,4}, {-1,0,0}},
            {{1,2,6,5}, {0,0,1}},
            {{2,3,7,6}, {1,0,0}},
            {{3,0,4,7}, {0,0,-1}},
            {{4,5,6,7}, {0,1,0}}
        }};
        static const std::array<int, 6> TRIANGLES = {0,1,2,0,2,3};
        for (unsigned faceId = 0; faceId < FACES.size(); faceId++) {
            auto& face = FACES[faceId];
            irr::video::S3DVertex vertex({0,0,0}, face.second, irr::video::SColor(255,255,255,255), {0,0});
            for (unsigned vertexId : face.first) {
                vertex.Pos = VERTICES[vertexId];
                meshBuffer->Vertices.push_back(vertex);
            }
            for (int index : TRIANGLES) {
                meshBuffer->Indices.push_back(4*faceId+index);
            }
        }

        return result;
    }

    /**
     * Generates the vertices of a square representing the given plane.
     *
     * The square will have a side length of 100. The indexes of the vertices in the output array
     * are organized like this:<br>
     * <code>
     * 0-1<br>
     * | |<br>
     * 2-3<br>
     * </code>
     *
     * @param[in] normal Normal vector of the plane.
     * @param[in] offset Offset of the plane from the origin (the point origin+normal*offset belongs to the plane).
     * @param[out] vertices Vertices generated by this function.
     */
    static void getPlaneVertices(const btVector3& normal, btScalar offset, std::array<irr::video::S3DVertex,4>& vertices) {
        static const btScalar planeSize = toIrrUnits(Scalar<SI::Length>(1000));
        btVector3 planVectorX = {0, normal.z(), -normal.y()};
        if (planVectorX.norm() < 0.2f) {
            planVectorX = {-normal.z(), 0, normal.x()};
        }
        planVectorX.normalize();
        btVector3 planVectorY = btCross(normal, planVectorX);

        btVector3 middle = normal*offset;
        for (int arrayX = 0; arrayX<=1; arrayX++) {
            btVector3 xTranslated = middle + planeSize * ((arrayX*2)-1)/2.f * planVectorX;
            for (int arrayY = 0; arrayY<=1; arrayY++) {
                btVector3 translated = xTranslated + planeSize * ((arrayY*2)-1)/2.f * planVectorY;
                irr::video::S3DVertex& vertex = vertices[2*arrayX+arrayY];
                vertex.Normal = {normal.x(), normal.y(), normal.z()};
                vertex.Pos = {translated.x(), translated.y(), translated.z()};
                vertex.Color = irr::video::SColor(255,255,255,255);
                vertex.TCoords = irr::core::vector2df(arrayX, arrayY);
            }
        }
    }

    /**
     * Generates an Irrlicht mesh from the mesh representation of the physics engine.
     *
     * @param vertices List of vertices of the mesh.
     * @param faces List of faces of the mesh.
     * @return An irrlicht shape built from the arguments.
     */
    static irrlicht_ptr<irr::scene::IMesh> makeMesh(const ConvexMesh& physicsMesh) {
        using namespace irr::scene;
        irrlicht_ptr<SMesh> result(new SMesh());
        irrlicht_ptr<SMeshBuffer> meshBuffer(new SMeshBuffer());
        result->addMeshBuffer(meshBuffer.get());

        unsigned index = 0;
        for (const auto& triangle : physicsMesh.getTriangles()) {
            irr::core::vector3df normal = btToIrrVector(triangle->getNormal());
            irr::video::S3DVertex vertex({0,0,0}, normal, irr::video::SColor(255,255,255,255), {0,0});
            for (const btVector3& physicsVertex : triangle->getVertices()) {
                vertex.Pos = btToIrrVector(physicsVertex);
                meshBuffer->Vertices.push_back(vertex);
                meshBuffer->Indices.push_back(index);
                index++;
            }
        }
        return result;
    }
};

GraphicObject::GraphicObject(const Body& body, irr::scene::ISceneManager& scene) :
    body(body),
    node(scene.addEmptySceneNode(nullptr))
{
    IrrlichtDrawer drawer(*node);
    body.drawShape(drawer);
    updateTransform(body.getEngineTransform());
    body.addMoveListener(*this);
}

void GraphicObject::onBodyMove(const btTransform& transform) {
    updateTransform(transform);
}

GraphicObject::~GraphicObject() {
    body.removeMoveListener(*this);
}

void GraphicObject::updateTransform(const btTransform& transform) {
    node->setPosition(btToIrrVector(transform.getOrigin()));
    irr::core::vector3df eulerAngles = btQuaternionToEulerAngles(transform.getRotation());
    node->setRotation(eulerAngles);
}
