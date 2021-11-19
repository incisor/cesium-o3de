#include <Cesium/TMSRasterOverlayComponent.h>
#include <Cesium3DTilesSelection/RasterOverlay.h>
#include <Cesium3DTilesSelection/TileMapServiceRasterOverlay.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace Cesium
{
    void TMSRasterOverlaySource::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<TMSRasterOverlaySource>()->Version(0)
                ->Field("url", &TMSRasterOverlaySource::m_url)
                ->Field("headers", &TMSRasterOverlaySource::m_headers)
                ->Field("fileExtension", &TMSRasterOverlaySource::m_fileExtension)
                ->Field("minimumLevel", &TMSRasterOverlaySource::m_minimumLevel)
                ->Field("maximumLevel", &TMSRasterOverlaySource::m_maximumLevel)
                ;
        }
    }

    TMSRasterOverlaySource::TMSRasterOverlaySource()
        : m_fileExtension{ "png" }
        , m_minimumLevel{0}
        , m_maximumLevel{25}
    {
    }

    void TMSRasterOverlayComponent::Reflect(AZ::ReflectContext* context)
    {
        TMSRasterOverlaySource::Reflect(context);

        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<TMSRasterOverlayComponent, AZ::Component>()->Version(0)
                ->Field("source", &TMSRasterOverlayComponent::m_source);
        }
    }

    void TMSRasterOverlayComponent::LoadRasterOverlay(const TMSRasterOverlaySource& source)
    {
        m_source = source;
        RasterOverlayComponent::LoadRasterOverlay();
    }

    std::unique_ptr<Cesium3DTilesSelection::RasterOverlay> TMSRasterOverlayComponent::LoadRasterOverlayImpl()
    {
        // setup TMS option
        Cesium3DTilesSelection::TileMapServiceRasterOverlayOptions options{};
        if (m_source.m_maximumLevel > m_source.m_minimumLevel)
        {
            options.minimumLevel = m_source.m_minimumLevel;
            options.maximumLevel = m_source.m_maximumLevel;
        }
        options.fileExtension = m_source.m_fileExtension.c_str();

        // setup TMS headers
        std::vector<CesiumAsync::IAssetAccessor::THeader> headers;
        for (const auto& header : m_source.m_headers)
        {
            headers.emplace_back(header.first.c_str(), header.second.c_str());
        }

        return std::make_unique<Cesium3DTilesSelection::TileMapServiceRasterOverlay>(
            "TMSRasterOverlay", m_source.m_url.c_str(), headers, options);
    }
} // namespace Cesium