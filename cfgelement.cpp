#include "cfgelement.h"

CFGElement::CFGElement(const QString &text, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_text(text), m_rect(rect) {}


QRectF CFGElement::boundingRect() const {
    return m_rect;
}

void CFGElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::darkBlue);
    painter->setPen(Qt::black);
    painter->drawRect(m_rect);
    painter->setPen(Qt::black);
    painter->drawText(m_rect, Qt::AlignCenter, m_text);
}

void CFGElement::connect(CFGElement *pointing_at) {
    m_outgoing.push_back(pointing_at);
    pointing_at->m_incoming.push_back(this);
}
