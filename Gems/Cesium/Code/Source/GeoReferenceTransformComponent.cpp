#include <Cesium/GeoReferenceTransformComponent.h>
#include <CesiumGeospatial/Transforms.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <glm/gtc/matrix_inverse.hpp>

namespace Cesium
{
    void GeoReferenceTransformComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<GeoReferenceTransformComponent, AZ::Component>()->Version(0);
        }
    }

    void GeoReferenceTransformComponent::Init()
    {
    }

    void GeoReferenceTransformComponent::Activate()
    {
        CoordinateTransformRequestBus::Handler::BusConnect(GetEntityId());
        m_enable = true;
        m_enableEvent.Signal(m_enable, m_config);
    }

    void GeoReferenceTransformComponent::Deactivate()
    {
        m_enable = false;
        m_enableEvent.Signal(m_enable, m_config);
        CoordinateTransformRequestBus::Handler::BusDisconnect();
    }

    void GeoReferenceTransformComponent::SetECEFCoordOrigin(const glm::dvec3& origin)
    {
        m_config.m_origin = origin;
        m_config.m_O3DEToECEF = CesiumGeospatial::Transforms::eastNorthUpToFixedFrame(origin);
        m_config.m_ECEFToO3DE = glm::affineInverse(m_config.m_O3DEToECEF);
        m_transformChangeEvent.Signal(m_config);
    }

    const glm::dvec3& GeoReferenceTransformComponent::GetECEFCoordOrigin() const
    {
        return m_config.m_origin;
    }

    const glm::dmat4& GeoReferenceTransformComponent::O3DEToECEF() const
    {
        return m_config.m_O3DEToECEF;
    }

    const glm::dmat4& GeoReferenceTransformComponent::ECEFToO3DE() const
    {
        return m_config.m_ECEFToO3DE;
    }

    glm::dmat4 GeoReferenceTransformComponent::CalculateO3DEToECEFAtOrigin(const glm::dvec3& origin) const
    {
        return CesiumGeospatial::Transforms::eastNorthUpToFixedFrame(origin);
    }

    glm::dmat4 GeoReferenceTransformComponent::CalculateECEFToO3DEAtOrigin(const glm::dvec3& origin) const
    {
        return glm::affineInverse(CalculateO3DEToECEFAtOrigin(origin));
    }

    const CoordinateTransformConfiguration& GeoReferenceTransformComponent::GetConfiguration() const
    {
        return m_config;
    }

    bool GeoReferenceTransformComponent::IsEnable() const
    {
        return m_enable;
    }

    void GeoReferenceTransformComponent::BindTransformChangeEventHandler(TransformChangeEvent::Handler& handler)
    {
        handler.Connect(m_transformChangeEvent);
    }

    void GeoReferenceTransformComponent::BindTransformEnableEventHandler(TransformEnableEvent::Handler& handler)
    {
        handler.Connect(m_enableEvent);
    }
} // namespace Cesium
