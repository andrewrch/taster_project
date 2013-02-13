/*

	Copyright 2010 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pipeline.hpp"

const glm::mat4& Pipeline::getVPTrans()
{
//    glm::mat4 cameraTranslationTrans, cameraRotateTrans, persProjTrans;

//    cameraTranslationTrans = glm::translate(glm::mat4, -camera.pos.x, -camera.pos.y, -camera.pos.z);
//    cameraRotateTrans = glm::rotate(glm::mat4(1.0f), .InitCameraTransform(m_camera.Target, m_camera.Up);
//    PersProjTrans.InitPersProjTransform(m_persProjInfo);
    
//    m_VPTtransformation = PersProjTrans * CameraRotateTrans * CameraTranslationTrans;
    return VPTtransformation;
}

const glm::mat4& Pipeline::getWorldTrans()
{
//    glm::mat4 scaleTrans, rotateTrans, translationTrans;

//    scaleTrans = glm::scale(glm::mat4(1.0f), scale);
//    rotateTrans = glm::rotate(glm::mat4(1.0f), 1.0f, rotateInfo);
//    translationTrans = glm::translate(glm::mat4(1.0f), worldPos);

//    worldTransformation = translationTrans * rotateTrans * scaleTrans;
    return worldTransformation;
}

const glm::mat4& Pipeline::getWVPTrans()
{
    getWorldTrans();
    getVPTrans();

    WVPtransformation = VPTtransformation * worldTransformation;
    return WVPtransformation;
}

