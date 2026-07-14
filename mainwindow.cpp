#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <vector> // to use vectors
#include <iostream> // for debugging
#include <cstdlib> // for rand() and srand()
#include <ctime>   // for srand() to seed the random number generator

#include "simulation.h" // simulations
#include "tests.h" // test functions

#include "secondwindow.h"
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    /* ###### Here you can change the number of pixels ##### */
    size_x = 75;
    size_y = 75;
    /* ##################################################### */

    // run the simple test for the dsf
    test_dsf();

    reefImage = QImage(size_x,size_y, QImage::Format_RGB32);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::preparePlot(int size_x, int size_y){
    // colors for the plot
    // it would have been better to actually use like a parameter/settings file and not define this in both windows
    color.push_back(qRgb(3, 150, 220)); // blue, Background
    color.push_back(qRgb(2, 240, 85)); // light green, turf algae
    color.push_back(qRgb(8, 120, 8)); // dark green, Macroalgae
    color.push_back(qRgb(240, 152, 2)); // orange, corals


    // labels to go with the colors
    labels = {"Background", "Turf Algae", "Macroalgae", "Corals"};

    // Resize reefImage to size_x by size_y
    reefImage = reefImage.scaled(size_x, size_y);

    // fill images with B
    reefImage.fill(color[0]);
    if (ui->graphicsView->scene() == nullptr) {
        ui->graphicsView->setScene(new QGraphicsScene);
    }


    /* ######## create a legend ####### */
    // ----- had some AI (ChatGPT 3.5) help for this part ----(it was like Mittelmäßig hilfreich)

    // Access the existing QChartView from your UI
    QChartView *chartView = ui->chartView;

    // Access the chart associated with the chart view
    QChart *chart = chartView->chart();

    // Set background to be transparent
    chart->setBackgroundBrush(Qt::transparent);

    // Create a layout for the legend
    QVBoxLayout *legendLayout = new QVBoxLayout;

    // Create legend items with colored rectangles and labels
    for (int i = 0; i < labels.size(); ++i) {
        QLabel *colorBox = new QLabel;
        colorBox->setFixedSize(20, 20); // Set the size of the color box
        colorBox->setAutoFillBackground(true); // Ensure the background is filled

        // Construct a QColor object from the QRgb value
        QColor col = QColor(color[i]);
        colorBox->setStyleSheet(QString("background-color: %1;").arg(col.name())); // Set the background color

        QLabel *label = new QLabel(labels[i]);

        QWidget *legendItem = new QWidget;
        QHBoxLayout *itemLayout = new QHBoxLayout;
        itemLayout->addWidget(colorBox); // Add the color box
        itemLayout->addWidget(label); // Add the label
        legendItem->setLayout(itemLayout);

        legendLayout->addWidget(legendItem);

        // Store the label pointer in the labelMap
        labelMap[labels[i]] = label;
    }


    // Set the legend layout to the chart view
    QWidget *legendWidget = new QWidget;
    legendWidget->setLayout(legendLayout);
    chartView->setViewport(legendWidget);

    // Show the chart view
    chartView->show();
}



//declared vector as const to ensure no modification here
void MainWindow::display_state(const vector<vector<int>> field){
    // get the number of rows
    int numRows = field.size();

    // if the vector is not empty, get the number of columns
    int numCols = (numRows > 0) ? field[0].size() : 0;

    int totalCells = numRows*numCols;

    /* ### draw the image ### */
    // go through field and set pixel color according to flower type
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            reefImage.setPixel(i, j, color[field[i][j]]);
        }
    }

    QImage imageBigger = reefImage.scaled(490, 490, Qt::KeepAspectRatio); // make image bigger
    //QImage imageBigger = reefImage.scaled(ui->graphicsView->size(), Qt::KeepAspectRatio);

    // add image to graphics view
    auto scene = ui->graphicsView->scene();
    scene->addPixmap(QPixmap::fromImage(imageBigger));


    /* ### change the legend ### */

    vector<int> occupancies = sim.get_occupancy(); // Get occupancy percentages from sim.get_occupancy()

    // calculate percentages
    vector<double> percentages;
    for (int i = 0; i < occupancies.size(); ++i) {
        double percent = (occupancies[i] / static_cast<double>(totalCells)) * 100.0;
        percentages.push_back(percent);
    }

    // iterate over the labels and update them with occupancy percentages
    for (int i = 0; i < labels.size(); ++i) {
        if (labelMap.contains(labels[i])) {
            QLabel *label = labelMap[labels[i]];
            label->setText(QString("%1\n(%2%)").arg(labels[i]).arg(percentages[i], 0, 'f', 2)); // only 2 digits
        }
    }

}




void MainWindow::on_step_and_Display_Button_clicked()
{  
    sim.step();
    display_state(sim.get_reefField());
}


void MainWindow::on_step_10_Button_clicked()
{
    //cout << "\n";
    //cout << test_dsf();

    for (int i = 0; i < 10; i++){
        sim.step();
        display_state(sim.get_reefField());
    }
    display_state(sim.get_reefField());
}

void MainWindow::on_init_Button_clicked()
{

    // enable all buttons and sliders, only after initialisation
    // to reduce the possibility of breaking something from the ui itself
    ui->step_and_Display_Button->setEnabled(true);
    ui->step_10_Button->setEnabled(true);
    ui->executeDisaster_pushbutton->setEnabled(true);
    ui->herbivorySlider->setEnabled(true);
    ui->destructionSlider->setEnabled(true);

    // prepare plot and initialize everything
    preparePlot(size_x,size_y);
    sim.init(size_x, size_y);

    // grab the herbivory - otherwise it will be reset to 0
    sim.set_nherb_percent(ui->herbivorySlider->value());

    display_state(sim.get_reefField());
}


void MainWindow::on_switchButton_clicked()
{
    hide(); // hide main window

    SecondWindow secondWindow; // create second window
    secondWindow.setModal(true); // make first window inaccesible
    secondWindow.exec();

    show(); // show main window again

    // second window lives only in this scope
}


void MainWindow::on_herbivorySlider_valueChanged(int value)
{
    int n = ui->herbivorySlider->value();
    sim.set_nherb_percent(n);
    ui->herbivoryLabel->setText("Herbivory = " + QString::number(n) + "%");
}


void MainWindow::on_destructionSlider_valueChanged(int value)
{
    int n = ui->destructionSlider->value();
    ui->destructionLabel->setText("Destruction = " + QString::number(n) + "%");

}


void MainWindow::on_executeDisaster_pushbutton_clicked()
{
    int n = ui->destructionSlider->value();
    float percentageFloat = static_cast<float>(n) / 100.0f;
    sim.catastrophe(percentageFloat);
    display_state(sim.get_reefField());
}
