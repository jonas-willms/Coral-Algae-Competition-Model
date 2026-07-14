#include "secondwindow.h"
#include "ui_secondwindow.h"
#include "simulation.h"

#include <iostream>

SecondWindow::SecondWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecondWindow)
{
    ui->setupUi(this);


    /* ###### Here you can change the number of pixels ##### */
    size_x = 30;
    size_y = 30;
    /* ##################################################### */


    // initialize the reefImage
    reefImage = QImage(100, 100, QImage::Format_RGB32);

    // Setting up the chart for plotting
    chart = new QChart();
    series = new QLineSeries();
    series->setColor(Qt::red);
    series->setName("Average #C/(#C+#M)");
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->axisX()->setTitleText("Herbivory [% of Area]");
    chart->axisY()->setTitleText("#C/(#C+#M)");
    chart->setTitle("<H2>Coral-Macroalgae cover fraction");
    ui->chartView->setChart(chart);

}

SecondWindow::~SecondWindow()
{
    delete ui;
}

void SecondWindow::preparePlot(int size_x, int size_y){
    color.push_back(qRgb(3, 150, 220)); // blue, Background
    color.push_back(qRgb(2, 240, 85)); // light green, small algae
    color.push_back(qRgb(8, 120, 8)); // dark green, Macro algae
    color.push_back(qRgb(240, 152, 2)); // oragnge, corals

    // Resize reefImage to size_x by size_y
    reefImage = reefImage.scaled(size_x, size_y);

    // fill images with B
    reefImage.fill(color[0]);
    if (ui->graphicsView->scene() == nullptr) {
        ui->graphicsView->setScene(new QGraphicsScene);
    }

}

//declared vector as const to ensure no modification here
void SecondWindow::display_state(const vector<vector<int>> field){
    // Get the number of rows
    int numRows = field.size();

    // If the vector is not empty, get the number of columns
    int numCols = (numRows > 0) ? field[0].size() : 0;

    // go through field and set pixel color according to flower type
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            reefImage.setPixel(i, j, color[field[i][j]]);
        }
    }

    // Calculate the target size for the image - without the factor it looks bad
    int targetWidth = static_cast<int>(ui->graphicsView->size().width() * 0.95);
    int targetHeight = static_cast<int>(ui->graphicsView->size().height() * 0.95);
    QSize targetSize(targetWidth, targetHeight);

    // Scale the image to the target size while maintaining aspect ratio
    QImage imageScaled = reefImage.scaled(targetSize, Qt::KeepAspectRatio);

    // Add the scaled image to the graphics view
    auto scene = ui->graphicsView->scene();
    scene->addPixmap(QPixmap::fromImage(imageScaled));

}

void SecondWindow::on_startSimulationsPushButton_clicked()
{

    preparePlot(size_x, size_y);

    Simulation sim;
    sim.init(size_x, size_y);

    int n_runs = ui->n_runs->value(); // runs per simulations
    int n_herb_max = ui->n_herb_max->value();
    int n_herb_min = ui->n_herb_min->value();
    int n_herb_stepsize = ui->n_herb_step_size->value();
    int n_years_eq = ui->n_years->value();
    int n_years_eval = ui->n_years_evaluation->value();

    int catastrope_every = ui->catastrope_spinBox->value();
    int destruction_probability = ui->destruction_spinBox->value();

    sim.change_catastrophe_params(catastrope_every, destruction_probability);

    float dt = sim.get_dt();
    int n_steps_eq = static_cast<int>(n_years_eq / dt + 0.5); // + 0.5 for correct rounding
    int n_steps_eval = static_cast<int>(n_years_eval / dt + 0.5); // + 0.5 for correct rounding
    QString progressLabelText;

    chart->axisX()->setRange(n_herb_min, n_herb_max);
    chart->axisY()->setRange(0, 1);

    series->clear(); // deletes all data points


    // before starting, check that it is reasonable
    if (n_herb_min >= n_herb_max){
        QMessageBox::warning(this, "No", "Check your minimal and maximal herbivory and try again");
        return;
    }

    for(int n_herb = n_herb_min; n_herb <= n_herb_max; n_herb = n_herb + n_herb_stepsize){


        int n_m = 0; // count of value 2 (n_m)
        int n_c = 0; // count of value 3 (n_c)
        double result = 0.0; // fraction for n_c/(n_c + n_m)

        /* --- iterate over n_runs results ---- */
        for(int j = 0; j < n_runs; j ++){

            // Set the reef to a random, equally distributed state.
            sim.reset_with_new_config();
            sim.set_nherb_percent(n_herb); // set herbivory


            // allow for equilibration
            for(int i = 0; i < n_steps_eq; i++){
                sim.step();
            }

            // average over all evaluated steps
            for(int i = 0; i < n_steps_eval; i++){
                sim.step();

                std::vector<int> occupancy = sim.get_occupancy();
                n_m += occupancy[2];
                n_c += occupancy[3];
            }




            /* --- display results ---- */
            display_state(sim.get_reefField()); // show the image

            //give information about the displayed image
            progressLabelText = "Displaying: "
                                + QString::number(j+1) + "/" + QString::number(n_runs)
                                + " Herbovory=" + QString::number(n_herb-n_herb_stepsize) + "%";
            ui->progress_label->setText(progressLabelText);

            // Allow the event loop to process events -> live feedback display
            QCoreApplication::processEvents();


        }
        //result = result/n_runs; // calculate average result
        result += static_cast<double>(n_c) / (n_c+n_m); // (a*s)/(a*s+b*s) = a/(a+b)

        series->append(n_herb, result); // add data point do plot
        ui->chartView->update(); // update the ui

        // Allow the event loop to process events -> looks nicer
        QCoreApplication::processEvents();

    }

}
