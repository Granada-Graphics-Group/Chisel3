#if defined(_WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#include <windows.h>
#endif

#include <QApplication>
#include <QDebug>

#include "glrenderer.h"
#include "mainwindow.h"
#include "openglwidget.h"
#include "logger.hpp"

#include <QDesktopWidget>
#include <QSurfaceFormat>
#include <QTimer>
#include <clocale>

int main(int argc, char** argv)
{
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);    
    
    QSurfaceFormat glFormat;
    glFormat.setVersion(4, 5);
    glFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
    glFormat.setRenderableType(QSurfaceFormat::OpenGL);
    glFormat.setOption(QSurfaceFormat::DebugContext | QSurfaceFormat::DeprecatedFunctions);
    QSurfaceFormat::setDefaultFormat(glFormat);    
    
    QApplication app(argc, argv);
    
    log_inst.set_thread_name("MAIN");    
    LOG("CHISel starting ...");
    
    std::setlocale(LC_NUMERIC, "C");
        
//     std::vector<glm::vec3> positionTexels {{0, 1, 0}, {0.3, -1, 0.8}, {0.8, 0.8, 1}, {-1, 0.7, 0}};
//     std::vector<glm::vec3> normalTexels {{0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {-1, 0, 0}};
//     std::vector<float> distanceData;
//     
//     auto originIndex = 0;
//     auto originPosition = positionTexels[originIndex];
//     auto originNormal = normalTexels[originIndex];
// 
//     for(auto i = 1; i < positionTexels.size(); ++i)
//     {
//         auto position = positionTexels[i];
//         auto faceNormal = normalTexels[i];
//         
//         if(originNormal == faceNormal)
//         {
//            distanceData[i] = glm::distance(originPosition, position); 
//         }
//         else if (originNormal + faceNormal != glm::vec3(0)) // contiguos faces
//         {
//             auto rotationAxis = glm::cross(originNormal, faceNormal);
// 
//             auto lower = originNormal + faceNormal - rotationAxis;
//             auto higher = originNormal + faceNormal + rotationAxis;
//             
//             auto halfSize = 1.0;
//             
//             auto positionOnOriginNormal = position * glm::abs(originNormal);
//             auto distance = halfSize - (positionOnOriginNormal.x + positionOnOriginNormal.y + positionOnOriginNormal.z);
//             auto rotatedPosition = glm::abs(originNormal) * originPosition + (halfSize + distance) * faceNormal + glm::abs(rotationAxis) * position;
//             
//             auto distanceOnRotationAxisHigher = glm::abs(rotatedPosition - higher) * glm::abs(rotationAxis);
//             auto distanceOnHigher = distanceOnRotationAxisHigher.x + distanceOnRotationAxisHigher.y + distanceOnRotationAxisHigher.z;
//             auto rotatedPosition90 = higher + distance * rotationAxis + distanceOnHigher * faceNormal;
//             
//             auto distanceOnRotationAxisLower = glm::abs(rotatedPosition - lower) * glm::abs(rotationAxis);
//             auto distanceOnLower = distanceOnRotationAxisLower.x + distanceOnRotationAxisLower.y + distanceOnRotationAxisLower.z;
//             auto rotatedPositionMinus90 = lower - distance * rotationAxis + distanceOnLower * faceNormal;
// 
//             std::array<float, 3> distances;
//             distances[0] = glm::distance(originPosition, rotatedPosition);
//             distances[1] = glm::distance(originPosition, rotatedPosition90);
//             distances[2] = glm::distance(originPosition, rotatedPositionMinus90);
// 
//             distanceData[i] = *std::min_element(begin(distances), end(distances));
//         }
//         else
//         {
//             std::array<glm::vec3, 5> originalPositions;
//             std::array<glm::vec3, 5> rotatedPositions;
//             originalPositions[0] = originPosition;
//             
//             glm::vec3 helperNormal {originNormal.y, originNormal.z, originNormal.x };            
//             auto rotationAxis = glm::cross(originNormal, helperNormal);
//             
//             auto halfSize = 1.0;            
// 
//             auto originPositionOnHelperNormal = originPosition * glm::abs(helperNormal);
//             auto positionOnHelperNormal = position * glm::abs(helperNormal);            
//             
//             auto ogDistance = halfSize - (originPositionOnHelperNormal.x + originPositionOnHelperNormal.y + originPositionOnHelperNormal.z);
//             auto distance = halfSize - (positionOnHelperNormal.x + positionOnHelperNormal.y + positionOnHelperNormal.z);
//             
//             rotatedPositions[0] = glm::abs(originNormal) * originPosition + (3 * halfSize + distance) * helperNormal + glm::abs(rotationAxis) * position;
//             
//             auto originalHigher = originNormal + helperNormal + rotationAxis;
//             auto higher = originNormal + 3 * halfSize * helperNormal + rotationAxis;
//             
//             for(char j = 0; j < 4; ++j)
//             {
//                 auto ogDistanceOnAxisHigher = glm::abs(originalPositions[j] - originalHigher) * glm::abs(rotationAxis);
//                 auto ogDistanceOnHigher = ogDistanceOnAxisHigher.x + ogDistanceOnAxisHigher.y + ogDistanceOnAxisHigher.z;
//                 originalPositions[j + 1] = originalHigher + ogDistance * rotationAxis - ogDistanceOnHigher * helperNormal;
//                 originalHigher += 2 * halfSize * rotationAxis;
//                 
//                 auto originPositionOnHelperNormal = originalPositions[j + 1] * glm::abs(helperNormal);
//                 ogDistance = halfSize - (originPositionOnHelperNormal.x + originPositionOnHelperNormal.y + originPositionOnHelperNormal.z);
//                                 
//                 auto distanceOnRotationAxisHigher = glm::abs(rotatedPositions[j] - higher) * glm::abs(rotationAxis);
//                 auto distanceOnHigher = distanceOnRotationAxisHigher.x + distanceOnRotationAxisHigher.y + distanceOnRotationAxisHigher.z;
//                 rotatedPositions[j + 1] = higher + distance * rotationAxis + distanceOnHigher * helperNormal;
//                 higher += 2 * halfSize * rotationAxis;
//                 
//                 auto positionOnHelperNormal = rotatedPositions[j + 1] * glm::abs(helperNormal);
//                 distance = glm::abs(3.0 * halfSize - (positionOnHelperNormal.x + positionOnHelperNormal.y + positionOnHelperNormal.z));
//             }
//             
//             std::vector<float> distances;
//             
//             for(char j = 0; j < 5; ++j)
//             {
//                 if(j != 0 && j != 5)
//                     distances.push_back(glm::distance(originalPositions[j], rotatedPositions[j - 1]));                    
//                 
//                 distances.push_back(glm::distance(originalPositions[j], rotatedPositions[j]));
//                 distances.push_back(glm::distance(originalPositions[j], rotatedPositions[j + 1]));             
//             }
//             
//             distanceData[i] = *std::min_element(begin(distances), end(distances));
//         }
//     }


    
    
    MainWindow mw;
    
    mw.show();
    mw.settingUp();
    QTimer::singleShot(50, &mw, &MainWindow::fixit);
    mw.move(QApplication::desktop()->screen()->rect().center() - mw.rect().center());
    
    return app.exec();
}
