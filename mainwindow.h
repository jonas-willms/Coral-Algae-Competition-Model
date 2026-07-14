#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtCharts>
#include <QLegend> // to create a legend
#include <vector> // to use vectors

#include "simulation.h"

using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    /**
     * @brief Display a 2d vector as an image
     * @param field Field to display
     */
    void display_state(const vector< vector<int>> field);

    /**
     * @brief Prepare the plot.
     * @param size_x
     * @param size_y
     */
    void preparePlot(int size_x, int size_y);



    // honestly - the names should tell what it is
    // I'm taking a bit less effort on the whole naming for button functions etc

    /**
     * @brief Button to make 10 time steps clicked
     */
    void on_step_10_Button_clicked();

    /**
     * @brief Button to do a step clicked.
     */
    void on_step_and_Display_Button_clicked();

    /**
     * @brief Button to initialize clicked
     */
    void on_init_Button_clicked();

    /**
     * @brief Button to switch windows clicked
     */
    void on_switchButton_clicked();

    /**
     * @brief on_herbivorySlider_valueChanged
     * @param value
     */
    void on_herbivorySlider_valueChanged(int value);

    /**
     * @brief on_executeDisaster_pushbutton_clicked
     */
    void on_executeDisaster_pushbutton_clicked();

    /**
     * @brief on_destructionSlider_valueChanged
     * @param value
     */
    void on_destructionSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    QImage reefImage; // to show a nice image


    QMap<QString, QLabel*> labelMap; // Store pointers to QLabel objects
    QList<QString> labels; // store labels

    std::vector<QRgb> color; // predefined colors for easier access

    Simulation sim; // simulation object


    int size_x, size_y; // size of the reef


};
#endif // MAINWINDOW_H
