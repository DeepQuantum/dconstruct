#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QScrollBar>
#include "cfg/cfgelement.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGraphicsScene *scene = new QGraphicsScene(this);

    scene->setBackgroundBrush(QBrush(Qt::white));

    ui->graphicsView->setScene(scene);
    ui->graphicsView->verticalScrollBar()->hide();
    ui->graphicsView->horizontalScrollBar()->hide();

    CFGElement *cfgs[4];
    int i = 0;
    for (auto &cfg : cfgs) {
        cfg = new CFGElement("Hello\ntest", QRectF(i, i/2, 50, 50));
        scene->addItem(cfg);
        i += 500;
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
