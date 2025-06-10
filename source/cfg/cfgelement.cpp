#include "cfgelement.h"

CFGElement::CFGElement(const QString &text, const QRectF rect, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_text(text), m_rect(rect) {}


void CFGElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::darkBlue);
    painter->setPen(Qt::black);
    painter->drawRect(m_rect);
    painter->setPen(Qt::black);
    //const int padding = 10;
    //QRectF textRect = m_rect.adjusted(padding, padding, -padding, -padding);
    painter->drawText(this->m_rect, Qt::AlignLeft | Qt::AlignVCenter, m_text);
}

void CFGElement::update_rect(const QRectF &rect) {
    prepareGeometryChange();
    this->m_rect = rect;
    update();
}

void CFGElement::connect(CFGElement *pointing_at) {
    m_outgoing.push_back(pointing_at);
    pointing_at->m_incoming.push_back(this);
}
