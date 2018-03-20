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
        scene.addSphereSceneNode(radius, 16, &rootNode);
    }

    IrrlichtDrawer(irr::scene::ISceneNode& rootNode) : rootNode(rootNode) {

    }
private:
    /** Node that will hold the 3d representation of a collision shape. */
    irr::scene::ISceneNode& rootNode;

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
        static const btScalar planeSize = 100.0f;
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
};

GraphicObject::GraphicObject(const Body& body, irr::scene::ISceneManager& scene) :
    body(body),
    node(scene.addEmptySceneNode(nullptr))
{
    IrrlichtDrawer drawer(*node);
    body.drawShape(drawer);
}
