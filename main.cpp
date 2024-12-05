#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFrame>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <limits>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>

#include "graph.h"
#include "ZoomableGraphicsView.h"

class MapDistanceApp : public QMainWindow
{
public:
    MapDistanceApp(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        // Central Widget and Main Layout
        QWidget *centralWidget = new QWidget(this);
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(15);

        // Map View
        // mapView = new QGraphicsView(this);
        // mapScene = new QGraphicsScene(this);

        mapView = new ZoomableGraphicsView(this);
        mapScene = new QGraphicsScene(this);
        mapView->setScene(mapScene);
        mapView->setFrameStyle(QFrame::NoFrame);
        mapView->setStyleSheet("border: 2px solid #8e44ad; border-radius: 5px;");

        // Info Frame
        QFrame *infoFrame = new QFrame(this);
        infoFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
        infoFrame->setStyleSheet("background-color: #ecf0f1; border: 2px solid #95a5a6; border-radius: 5px;");
        QVBoxLayout *infoPanelLayout = new QVBoxLayout(infoFrame);
        infoPanelLayout->setContentsMargins(10, 10, 10, 10);

        // Header Label
        QLabel *headerLabel = new QLabel("Distance Information", this);
        headerLabel->setAlignment(Qt::AlignCenter);
        headerLabel->setStyleSheet("background-color: #34495e; color: white; font-size: 16px; padding: 5px;");

        // Labels
        QLabel *point1Label = createSubtitleLabel("Point 1 Coordinates:");
        QLabel *point2Label = createSubtitleLabel("Point 2 Coordinates:");
        QLabel *modelDistanceLabel = createSubtitleLabel("Approx Distance (Model):");
        QLabel *realDistanceLabel = createSubtitleLabel("Real Distance:");

        point1Coord = createValueLabel("<i>Not selected</i>");
        point2Coord = createValueLabel("<i>Not selected</i>");
        modelDistance = createValueLabel("<i>Not calculated</i>");
        realDistance = createValueLabel("<i>Not calculated</i>");

        // Calculate Button
        calculateButton = new QPushButton("Calculate Distance", this);
        calculateButton->setStyleSheet(
            "background-color: #27ae60; color: white; font-weight: bold; padding: 8px; border-radius: 5px;"
            "hover { background-color: #2ecc71; }");
        connect(calculateButton, &QPushButton::clicked, this, &MapDistanceApp::calculateDistance);

        // Layout Setup
        infoPanelLayout->addWidget(headerLabel);
        infoPanelLayout->addSpacing(10);
        infoPanelLayout->addWidget(point1Label);
        infoPanelLayout->addWidget(point1Coord);
        infoPanelLayout->addWidget(point2Label);
        infoPanelLayout->addWidget(point2Coord);
        infoPanelLayout->addWidget(modelDistanceLabel);
        infoPanelLayout->addWidget(modelDistance);
        infoPanelLayout->addWidget(realDistanceLabel);
        infoPanelLayout->addWidget(realDistance);
        infoPanelLayout->addStretch();
        infoPanelLayout->addWidget(calculateButton);

        // Adding Widgets to Main Layout
        mainLayout->addWidget(mapView, 2);
        mainLayout->addWidget(infoFrame, 1);

        // Set Central Widget and Window Properties
        setCentralWidget(centralWidget);
        setWindowTitle("Map Distance Model");
        resize(900, 600);

        // Configuraciones importantes para zoom y scroll
        mapView->setRenderHint(QPainter::Antialiasing);
        mapView->setDragMode(QGraphicsView::ScrollHandDrag);
        mapView->setInteractive(true);
        mapView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
        mapView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        mapView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        mapView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);

        // Habilitar zoom con rueda del mouse
        mapView->setMouseTracking(true);
        mapView->viewport()->installEventFilter(this);
    }

    void renderGraph(const Graph &graph)
    {
        mapScene->clear();

        // Encontrar rangos de coordenadas
        double minLon = std::numeric_limits<double>::max();
        double maxLon = std::numeric_limits<double>::lowest();
        double minLat = std::numeric_limits<double>::max();
        double maxLat = std::numeric_limits<double>::lowest();

        const std::vector<Node> &nodes = graph.getNodes();
        for (const Node &node : nodes)
        {
            minLon = std::min(minLon, node.longitude);
            maxLon = std::max(maxLon, node.longitude);
            minLat = std::min(minLat, node.latitude);
            maxLat = std::max(maxLat, node.latitude);
        }

        // Dimensiones de la vista
        double viewWidth = mapView->width();
        double viewHeight = mapView->height();

        // Escalamiento
        double scaleX = viewWidth / (maxLon - minLon + 1e-6);
        double scaleY = viewHeight / (maxLat - minLat + 1e-6);

        // Funciones de mapeo
        auto mapX = [&](double lon) {
            return (lon - minLon) * scaleX;
        };

        auto mapY = [&](double lat) {
            return (lat - minLat) * scaleY;
        };

        // Dibujar aristas
        QPen edgePen(QColor(100, 100, 255), 2);
        for (const Edge &edge : graph.getEdges())
        {
            const Node &node1 = graph.getNode(edge.node1);
            const Node &node2 = graph.getNode(edge.node2);

            mapScene->addLine(
                mapX(node1.longitude), mapY(node1.latitude),
                mapX(node2.longitude), mapY(node2.latitude),
                edgePen);
        }

        // Dibujar nodos
        for (const Node &node : nodes)
        {
            QGraphicsEllipseItem *ellipse = mapScene->addEllipse(
                mapX(node.longitude) - 2, mapY(node.latitude) - 2,
                4, 4, // Tamaño ajustado
                QPen(Qt::red), QBrush(Qt::red));
            ellipse->setData(0, node.id);
        }

        // Ajustar vista
        mapView->fitInView(mapScene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }


protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QMainWindow::resizeEvent(event);
        mapView->fitInView(mapScene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }

private slots:
    void calculateDistance()
    {
        // Example Logic for Calculations
        point1Coord->setText("<b>Lat:</b> 40.7128, <b>Lon:</b> -74.0060");
        point2Coord->setText("<b>Lat:</b> 34.0522, <b>Lon:</b> -118.2437");
        modelDistance->setText("<b>3.94 km</b>");
        realDistance->setText("<b>4.12 km</b>");
    }

private:
    QGraphicsView *mapView;
    QGraphicsScene *mapScene;
    QLabel *point1Coord;
    QLabel *point2Coord;
    QLabel *modelDistance;
    QLabel *realDistance;
    QPushButton *calculateButton;

    QLabel *createSubtitleLabel(const QString &text)
    {
        QLabel *label = new QLabel(text, this);
        label->setStyleSheet("background-color: #bdc3c7; color: black; font-weight: bold; padding: 5px;");
        return label;
    }

    QLabel *createValueLabel(const QString &text)
    {
        QLabel *label = new QLabel(text, this);
        label->setStyleSheet("background-color: white; color: black; padding: 5px; border: 1px solid #95a5a6;");
        return label;
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Graph graph;
    // Graph graph("/home/aaroncs25/UTEC/EDA/eda-project-gui/nodes.txt", "/home/aaroncs25/UTEC/EDA/eda-project-gui/edges.txt");

    MapDistanceApp window;
    window.renderGraph(graph);
    window.show();
    return app.exec();
}
