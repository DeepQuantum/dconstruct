#ifndef CFGELEMENT_H
#define CFGELEMENT_H

#include <QGraphicsItem>
#include <QPainter>
#include <vector>
#include <QTextEdit>


#include "../disassembly/instructions.h"

class CFGElement : public QGraphicsItem {
public:
    CFGElement(const QString &text, QRectF rect, QGraphicsItem *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void connect(CFGElement *pointing_at);
    void update_rect(const QRectF &rect);
     
    QRectF boundingRect() const override {
        return m_rect;
    }

    u32 m_lineCount;
    QString m_text;
    QRectF m_rect;
    std::vector<CFGElement*> m_incoming{};
    std::vector<CFGElement*> m_outgoing{};
};

#endif // LABELEDRECTITEM_H
