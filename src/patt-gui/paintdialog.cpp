#include "paintdialog.hpp"
#include "paintrenderer.hpp"

#include <QComboBox>
#include <QSpinBox>
#include <QLabel>

const int IdRole = Qt::UserRole;

PaintDialog::PaintDialog()
 {
     renderArea = new PaintRenderer;

     shapeComboBox = new QComboBox;
     shapeComboBox->addItem(tr("Circle"), SHAPE_CIRCLE);
     shapeComboBox->addItem(tr("Square"), SHAPE_SQUARE);
     shapeComboBox->addItem(tr("Up Triangle"), SHAPE_TRIANGLE_UP);
     shapeComboBox->addItem(tr("Down Triangle"), SHAPE_TRIANGLE_DOWN);
     shapeComboBox->addItem(tr("Diamond"), SHAPE_DIAMOND);
     shapeComboBox->addItem(tr("Hexagon"), SHAPE_HEXAGON);
     shapeComboBox->addItem(tr("Left Triangle"), SHAPE_TRIANGLE_LEFT);
     shapeComboBox->addItem(tr("Right Triangle"), SHAPE_TRIANGLE_RIGHT);
     shapeComboBox->addItem(tr("Plus"), SHAPE_PLUS);
     shapeComboBox->addItem(tr("Cross"), SHAPE_CROSS);

     shapeLabel = new QLabel(tr("&Shape:"));
     shapeLabel->setBuddy(shapeComboBox);

     penWidthSpinBox = new QSpinBox;
     penWidthSpinBox->setRange(0, 20);
 #if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_SIMULATOR)
     penWidthSpinBox->setSpecialValueText(tr("0"));
 #else
     penWidthSpinBox->setSpecialValueText(tr("0 (cosmetic pen)"));
 #endif

     penWidthLabel = new QLabel(tr("Pen &Width:"));
     penWidthLabel->setBuddy(penWidthSpinBox);

     penStyleComboBox = new QComboBox;
     penStyleComboBox->addItem(tr("Solid"), static_cast<int>(Qt::SolidLine));
     penStyleComboBox->addItem(tr("Dash"), static_cast<int>(Qt::DashLine));
     penStyleComboBox->addItem(tr("Dot"), static_cast<int>(Qt::DotLine));
     penStyleComboBox->addItem(tr("Dash Dot"), static_cast<int>(Qt::DashDotLine));
     penStyleComboBox->addItem(tr("Dash Dot Dot"), static_cast<int>(Qt::DashDotDotLine));
     penStyleComboBox->addItem(tr("None"), static_cast<int>(Qt::NoPen));

     penStyleLabel = new QLabel(tr("&Pen Style:"));
     penStyleLabel->setBuddy(penStyleComboBox);

     penCapComboBox = new QComboBox;
     penCapComboBox->addItem(tr("Flat"), static_cast<int>(Qt::FlatCap));
     penCapComboBox->addItem(tr("Square"), static_cast<int>(Qt::SquareCap));
     penCapComboBox->addItem(tr("Round"), static_cast<int>(Qt::RoundCap));

     penCapLabel = new QLabel(tr("Pen &Cap:"));
     penCapLabel->setBuddy(penCapComboBox);

     penJoinComboBox = new QComboBox;
     penJoinComboBox->addItem(tr("Miter"), static_cast<int>(Qt::MiterJoin));
     penJoinComboBox->addItem(tr("Bevel"), static_cast<int>(Qt::BevelJoin));
     penJoinComboBox->addItem(tr("Round"), static_cast<int>(Qt::RoundJoin));

     penJoinLabel = new QLabel(tr("Pen &Join:"));
     penJoinLabel->setBuddy(penJoinComboBox);

     brushStyleComboBox = new QComboBox;
     brushStyleComboBox->addItem(tr("Linear Gradient"),
             static_cast<int>(Qt::LinearGradientPattern));
     brushStyleComboBox->addItem(tr("Radial Gradient"),
             static_cast<int>(Qt::RadialGradientPattern));
     brushStyleComboBox->addItem(tr("Conical Gradient"),
             static_cast<int>(Qt::ConicalGradientPattern));
     //brushStyleComboBox->addItem(tr("Texture"), Qt::TexturePattern);
     brushStyleComboBox->addItem(tr("Solid"), static_cast<int>(Qt::SolidPattern));
     brushStyleComboBox->addItem(tr("Horizontal"), static_cast<int>(Qt::HorPattern));
     brushStyleComboBox->addItem(tr("Vertical"), static_cast<int>(Qt::VerPattern));
     brushStyleComboBox->addItem(tr("Cross"), static_cast<int>(Qt::CrossPattern));
     brushStyleComboBox->addItem(tr("Backward Diagonal"), static_cast<int>(Qt::BDiagPattern));
     brushStyleComboBox->addItem(tr("Forward Diagonal"), static_cast<int>(Qt::FDiagPattern));
     brushStyleComboBox->addItem(tr("Diagonal Cross"), static_cast<int>(Qt::DiagCrossPattern));
     brushStyleComboBox->addItem(tr("Dense 1"), static_cast<int>(Qt::Dense1Pattern));
     brushStyleComboBox->addItem(tr("Dense 2"), static_cast<int>(Qt::Dense2Pattern));
     brushStyleComboBox->addItem(tr("Dense 3"), static_cast<int>(Qt::Dense3Pattern));
     brushStyleComboBox->addItem(tr("Dense 4"), static_cast<int>(Qt::Dense4Pattern));
     brushStyleComboBox->addItem(tr("Dense 5"), static_cast<int>(Qt::Dense5Pattern));
     brushStyleComboBox->addItem(tr("Dense 6"), static_cast<int>(Qt::Dense6Pattern));
     brushStyleComboBox->addItem(tr("Dense 7"), static_cast<int>(Qt::Dense7Pattern));
     brushStyleComboBox->addItem(tr("None"), static_cast<int>(Qt::NoBrush));

     brushStyleLabel = new QLabel(tr("&Brush:"));
     brushStyleLabel->setBuddy(brushStyleComboBox);

     otherOptionsLabel = new QLabel(tr("Options:"));
     antialiasingCheckBox = new QCheckBox(tr("&Antialiasing"));
     transformationsCheckBox = new QCheckBox(tr("&Transformations"));

     connect(shapeComboBox, SIGNAL(activated(int)),
             this, SLOT(shapeChanged()));
     connect(penWidthSpinBox, SIGNAL(valueChanged(int)),
             this, SLOT(penChanged()));
     connect(penStyleComboBox, SIGNAL(activated(int)),
             this, SLOT(penChanged()));
     connect(penCapComboBox, SIGNAL(activated(int)),
             this, SLOT(penChanged()));
     connect(penJoinComboBox, SIGNAL(activated(int)),
             this, SLOT(penChanged()));
     connect(brushStyleComboBox, SIGNAL(activated(int)),
             this, SLOT(brushChanged()));
     connect(antialiasingCheckBox, SIGNAL(toggled(bool)),
             renderArea, SLOT(setAntialiased(bool)));
     connect(transformationsCheckBox, SIGNAL(toggled(bool)),
             renderArea, SLOT(setTransformed(bool)));

     QGridLayout *mainLayout = new QGridLayout;
 #if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_SIMULATOR)
     mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
 #endif
     mainLayout->setColumnStretch(0, 1);
     mainLayout->setColumnStretch(3, 1);
     mainLayout->addWidget(renderArea, 0, 0, 1, 4);
     mainLayout->addWidget(shapeLabel, 2, 0, Qt::AlignRight);
     mainLayout->addWidget(shapeComboBox, 2, 1);
     mainLayout->addWidget(penWidthLabel, 3, 0, Qt::AlignRight);
     mainLayout->addWidget(penWidthSpinBox, 3, 1);
     mainLayout->addWidget(penStyleLabel, 4, 0, Qt::AlignRight);
     mainLayout->addWidget(penStyleComboBox, 4, 1);
     mainLayout->addWidget(penCapLabel, 3, 2, Qt::AlignRight);
     mainLayout->addWidget(penCapComboBox, 3, 3);
     mainLayout->addWidget(penJoinLabel, 2, 2, Qt::AlignRight);
     mainLayout->addWidget(penJoinComboBox, 2, 3);
     mainLayout->addWidget(brushStyleLabel, 4, 2, Qt::AlignRight);
     mainLayout->addWidget(brushStyleComboBox, 4, 3);
     mainLayout->addWidget(otherOptionsLabel, 5, 0, Qt::AlignRight);
     mainLayout->addWidget(antialiasingCheckBox, 5, 1, 1, 1, Qt::AlignRight);
     mainLayout->addWidget(transformationsCheckBox, 5, 2, 1, 2, Qt::AlignRight);
     setLayout(mainLayout);

     shapeChanged();
     penChanged();
     brushChanged();
     antialiasingCheckBox->setChecked(true);

     setWindowTitle(tr("Basic Drawing"));
 }

 void PaintDialog::shapeChanged()
 {
     PaintRenderer::Shape shape = PaintRenderer::Shape(shapeComboBox->itemData(
             shapeComboBox->currentIndex(), IdRole).toInt());
     renderArea->setShape(shape);
 }

 void PaintDialog::penChanged()
 {
     int width = penWidthSpinBox->value();
     Qt::PenStyle style = Qt::PenStyle(penStyleComboBox->itemData(
             penStyleComboBox->currentIndex(), IdRole).toInt());
     Qt::PenCapStyle cap = Qt::PenCapStyle(penCapComboBox->itemData(
             penCapComboBox->currentIndex(), IdRole).toInt());
     Qt::PenJoinStyle join = Qt::PenJoinStyle(penJoinComboBox->itemData(
             penJoinComboBox->currentIndex(), IdRole).toInt());

     renderArea->setPen(QPen(Qt::blue, width, style, cap, join));
 }

 void PaintDialog::brushChanged()
 {
     //QColor brushColor = renderArea;

     Qt::BrushStyle style = Qt::BrushStyle(brushStyleComboBox->itemData(
             brushStyleComboBox->currentIndex(), IdRole).toInt());

     if (style == Qt::LinearGradientPattern) {
         QLinearGradient linearGradient(0, 0, 100, 100);
         linearGradient.setColorAt(0.0, Qt::white);
         linearGradient.setColorAt(0.2, Qt::green);
         linearGradient.setColorAt(1.0, Qt::black);
         renderArea->setBrush(linearGradient);
     } else if (style == Qt::RadialGradientPattern) {
         QRadialGradient radialGradient(50, 50, 50, 70, 70);
         radialGradient.setColorAt(0.0, Qt::white);
         radialGradient.setColorAt(0.2, Qt::green);
         radialGradient.setColorAt(1.0, Qt::black);
         renderArea->setBrush(radialGradient);
     } else if (style == Qt::ConicalGradientPattern) {
         QConicalGradient conicalGradient(50, 50, 150);
         conicalGradient.setColorAt(0.0, Qt::white);
         conicalGradient.setColorAt(0.2, Qt::green);
         conicalGradient.setColorAt(1.0, Qt::black);
         renderArea->setBrush(conicalGradient);
     } else if (style == Qt::TexturePattern) {
         renderArea->setBrush(QBrush(QPixmap(":/images/brick.png")));
     } else {
         renderArea->setBrush(QBrush(Qt::green, style));
     }
 }

PaintDialog::~PaintDialog()
{

}
