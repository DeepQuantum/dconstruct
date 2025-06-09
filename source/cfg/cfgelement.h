#ifndef CFGELEMENT_H
#define CFGELEMENT_H

#include <QGraphicsItem>
#include <QPainter>
#include <vector>

#include "../disassembly/instructions.h"

class CFGElement : public QGraphicsItem {
public:
    CFGElement(const QString &text, const QRectF &rect, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void connect(CFGElement *pointing_at);

private:
    QString m_text;
    QRectF m_rect;
    std::vector<CFGElement*> m_incoming{};
    std::vector<CFGElement*> m_outgoing{};
};

#endif // LABELEDRECTITEM_H
