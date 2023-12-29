#include "helpbrowser.h"
#include "edge.h"
#include "mscene.h"
InformationDialog::InformationDialog(const QString& strPath, const QString& strFileName,
                                     QWidget* pwgt)
    : QDialog(pwgt)
{
    setWindowFlags(Qt::Dialog);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint |
                   Qt::WindowCloseButtonHint);
    setMinimumSize(700, 400);

    setWindowModality(Qt::WindowModal);
    setWindowIcon(QIcon(":/icons/help.png"));
    setWindowTitle("Справка");

    QPushButton* pcmdBack = new QPushButton("<<");
    QPushButton* pcmdHome = new QPushButton("Help");
    QPushButton* pcmdForward = new QPushButton(">>");
    QTextBrowser* ptxtBrowser = new QTextBrowser;

    connect(pcmdBack, SIGNAL(clicked()), ptxtBrowser, SLOT(backward()));
    connect(pcmdHome, SIGNAL(clicked()), ptxtBrowser, SLOT(home()));
    connect(pcmdForward, SIGNAL(clicked()), ptxtBrowser, SLOT(forward()));
    connect(ptxtBrowser, SIGNAL(backwardAvailable(bool)), pcmdBack, SLOT(setEnabled(bool)));
    connect(ptxtBrowser, SIGNAL(forwardAvailable(bool)), pcmdForward, SLOT(setEnabled(bool)));

    ptxtBrowser->setSearchPaths(QStringList() << strPath);
    ptxtBrowser->setSource(QString("main.htm"));
    ptxtBrowser->setSource(strFileName);
    // layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    QHBoxLayout* phbxLayout = new QHBoxLayout;
    phbxLayout->addWidget(pcmdBack);
    phbxLayout->addWidget(pcmdHome);
    phbxLayout->addWidget(pcmdForward);
    pvbxLayout->addLayout(phbxLayout);
    pvbxLayout->addWidget(ptxtBrowser);
    setLayout(pvbxLayout);

    name = "faq";
    readSettings();
}

InformationDialog::InformationDialog(const QString& text_view)
{
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowIcon(QIcon(":/icons/graph.png"));
    setFixedSize(300, 400);

    QPushButton* saveButton = new QPushButton("Save");
    QPushButton* rejectButton = new QPushButton("Cancel");
    QPlainTextEdit* browser = new QPlainTextEdit;
    browser->setReadOnly(true);
    browser->setPlainText(text_view);

    connect(saveButton, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(rejectButton, SIGNAL(clicked(bool)), SLOT(reject()));

    QGridLayout* lay = new QGridLayout;
    lay->addWidget(browser, 0, 0, 1, 2);
    lay->addWidget(saveButton, 1, 0);
    lay->addWidget(rejectButton, 1, 1);
    setLayout(lay);

    name = "text_view";
    readSettings();
}

bool InformationDialog::outDialog(const QString& text, QString title)
{
    InformationDialog* temp = new InformationDialog(text);
    temp->setWindowTitle(title);
    temp->show();
    if (temp->exec() == QDialog::Accepted)
    {
        delete temp;
        return true;
    }
    delete temp;
    return false;
}

InformationDialog::InformationDialog(MScene* scene, QWidget* par)
{
    scene_for_set = scene;
    new_diam = scene->diameter();
    oriented = scene->isOriented();
    arr_size = scene->arrSize();
    if (par)
        move(par->pos());
    setParent(par);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::Window);

    setWindowIcon(QIcon(":/icons/graph.png"));
    setFixedSize(300, 400);
    setWindowTitle("Settings");

    QVBoxLayout* main_lay = new QVBoxLayout;

    QGroupBox* diam = new QGroupBox("Node diameter");
    QHBoxLayout* diam_lay = new QHBoxLayout;
    diam->setLayout(diam_lay);
    QSlider* diam_slider = new QSlider(Qt::Horizontal);
    QLabel* diam_edit = new QLabel;
    diam_edit->setFixedWidth(20);
    diam_edit->setNum(scene->diameter());
    diam_slider->setRange(30, 120);
    diam_slider->setValue(scene->diameter());
    diam_slider->setSingleStep(1);
    diam_slider->setPageStep(5);
    diam_slider->setTickInterval(5);
    diam_slider->setTickPosition(QSlider::TicksBelow);
    connect(diam_slider, SIGNAL(valueChanged(int)), diam_edit, SLOT(setNum(int)));
    connect(diam_slider, &QSlider::valueChanged, this, &InformationDialog::set_new_diam);
    diam_lay->addWidget(diam_slider);
    diam_lay->addWidget(diam_edit);

    QGroupBox* ori = new QGroupBox("Oriented graph");
    QHBoxLayout* ori_lay = new QHBoxLayout;
    ori->setLayout(ori_lay);
    QRadioButton* yes = new QRadioButton("Yes");
    QRadioButton* no = new QRadioButton("No");
    if (oriented)
        yes->setChecked(true);
    else
        no->setChecked(true);
    connect(yes, &QRadioButton::toggled, this, &InformationDialog::set_oriented);
    ori_lay->addWidget(yes);
    ori_lay->addWidget(no);

    QGroupBox* arrow_size = new QGroupBox("Arrow size");
    QHBoxLayout* arrow_size_lay = new QHBoxLayout;
    arrow_size->setLayout(arrow_size_lay);
    QSlider* arrow_size_slider = new QSlider(Qt::Horizontal);
    QLabel* arrow_size_edit = new QLabel;
    arrow_size_edit->setFixedWidth(20);
    arrow_size_edit->setNum(scene->arrSize());
    arrow_size_slider->setRange(10, 60);
    arrow_size_slider->setValue(scene->arrSize());
    arrow_size_slider->setSingleStep(1);
    arrow_size_slider->setPageStep(5);
    arrow_size_slider->setTickInterval(10);
    arrow_size_slider->setTickPosition(QSlider::TicksBelow);
    connect(arrow_size_slider, SIGNAL(valueChanged(int)), arrow_size_edit, SLOT(setNum(int)));
    connect(arrow_size_slider, &QSlider::valueChanged, this, &InformationDialog::set_new_arr);
    arrow_size_lay->addWidget(arrow_size_slider);
    arrow_size_lay->addWidget(arrow_size_edit);


    QHBoxLayout* but = new QHBoxLayout;
    QPushButton* ok_but = new QPushButton("Ok");
    QPushButton* appl_but = new QPushButton("Apply");
    connect(ok_but, &QPushButton::clicked, this, &InformationDialog::ok_changes);
    connect(appl_but, &QPushButton::clicked, this, &InformationDialog::appl_changes);
    but->addWidget(ok_but);
    but->addWidget(appl_but);


    main_lay->addWidget(diam);
    main_lay->addWidget(ori);
    main_lay->addWidget(arrow_size);

    main_lay->addStretch();
    main_lay->addLayout(but);
    setLayout(main_lay);
    setWindowIcon(QIcon(":/icons/settings.png"));

    name = "settings";
    readSettings();
}


InformationDialog::InformationDialog(QWidget* par) : QDialog(par)
{}

void InformationDialog::ok_changes()
{
    appl_changes();
    this->close();
}

void InformationDialog::appl_changes()
{
    scene_for_set->setDiameter(new_diam);
    scene_for_set->setOrientated(oriented);
    scene_for_set->setArrowSize(arr_size);
    foreach(QGraphicsItem* it, scene_for_set->items())
    {
        if (it->type() == Edge::Type)
            ((Edge*)it)->adjust();
    }
    scene_for_set->update();
}

void InformationDialog::writeSettings()
{
    QSettings settings("LETI", "Graph application");
    settings.setValue(name, saveGeometry());
}

void InformationDialog::readSettings()
{
    QSettings settings("LETI", "Graph application");
    restoreGeometry(settings.value(name).toByteArray());
}

void InformationDialog::closeEvent(QCloseEvent* event)
{
    writeSettings();
    event->accept();
}
