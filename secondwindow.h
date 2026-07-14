#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QDialog>
#include <QtCharts>
#include <vector>
#include <QMessageBox>

using namespace std;


namespace Ui {
class SecondWindow;
}

class SecondWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SecondWindow(QWidget *parent = nullptr);
    ~SecondWindow();

private slots:
    /**
     * @brief Starts to run multiple simulations with varying herbivory.
     */
    void on_startSimulationsPushButton_clicked();

    /**
     * @brief Prepare the plotting
     * @param size_x
     * @param size_y
     */
    void preparePlot(int size_x, int size_y);

    /**
     * @brief Display a field
     * @param field
     */
    void display_state(const vector<vector<int>> field);


private:
    Ui::SecondWindow *ui;

    QLineSeries *series; // represents storage for the data points
    QChart *chart; // represents a chart

    QImage reefImage; // reef image
    std::vector<QRgb> color; // colors for easier access

    int size_x, size_y; // size of the simulated reef I put it here so you can acces it easily when the window is created
};

#endif // SECONDWINDOW_H
