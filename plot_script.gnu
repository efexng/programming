set title 'Linear Regression'
set xlabel 'Glucose'
set ylabel 'Heart Rate'
plot 'data_plot.txt' with points title 'Data Points', -0.33 * x + 203.49 title 'Linear Regression'
