#pragma once

#include <QGraphicsView>
#include "cfgelement.h"

class GraphicsViewController {
public:
    GraphicsViewController(QGraphicsView *view, QGraphicsScene *scene, const FunctionDisassembly *function);
    void start_draw() noexcept;

private:
    std::unique_ptr<CFGElement> m_head;
    QGraphicsView *m_graphicsView;
    QGraphicsScene *m_graphicsScene;
    std::vector<std::unique_ptr<CFGElement>> m_nodes;
    const FunctionDisassembly *m_currentFunction;
};