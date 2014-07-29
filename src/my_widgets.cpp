#include <QCursor>
#include <QHBoxLayout>
#include <QSizeGrip>
#include <QGroupBox>
#include "my_widgets.h"

//////////////////////////////////////////////////////////////////////////
// Selection area
SelectionArea::SelectionArea(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::SubWindow);
    setCursor(QCursor(Qt::CursorShape::SizeAllCursor));

    rubberband = new QRubberBand(QRubberBand::Rectangle, this);
    rubberband->show();
}

void SelectionArea::addSizeGrip()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(QMargins());
    layout->addWidget(new QSizeGrip(this), 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(new QSizeGrip(this), 0, Qt::AlignRight | Qt::AlignBottom);
}

void SelectionArea::mousePressEvent(QMouseEvent *ev)
{
    origin = ev->pos();
}

void SelectionArea::mouseMoveEvent(QMouseEvent *ev)
{
    if (ev->buttons() & Qt::LeftButton) {
        QPoint delta = ev->pos() - origin;
        move(pos() + delta);
    }
}

void SelectionArea::mouseReleaseEvent(QMouseEvent *ev)
{}

void SelectionArea::resizeEvent(QResizeEvent *)
{
    rubberband->resize(size());
}

//////////////////////////////////////////////////////////////////////////
// Image label
void ImageLabel::mousePressEvent(QMouseEvent *ev)
{
    origin = ev->pos();

    selection_area.reset(new SelectionArea(this));
}

void ImageLabel::mouseMoveEvent(QMouseEvent *ev)
{
    if (ev->buttons() & Qt::LeftButton) {
        selection_area->setGeometry(QRect(origin, ev->pos()).normalized());
        selection_area->show();
    }
    this->repaint();
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    selection_area->addSizeGrip();
}

void ImageLabel::setImage(const QImage& image)
{
    if (org_sz.isEmpty()) org_sz = size();
    setMaximumSize(org_sz);
    resize(org_sz);

    float h_ratio = (float)size().height() / image.height();
    float w_ratio = (float)size().width()  / image.width();
    ratio = w_ratio < h_ratio ? w_ratio : h_ratio;

    img = image.scaled(image.size() * ratio,
                       Qt::IgnoreAspectRatio,
                       Qt::SmoothTransformation);

    setPixmap(QPixmap::fromImage(img));
}

bool ImageLabel::getSelectionArea(QRect& area)
{
    if (selection_area && selection_area->width()) {
        area = QRect(selection_area->pos(), selection_area->size());
        return true;
    }

    area = QRect();
    return false;
}