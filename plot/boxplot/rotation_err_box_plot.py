#
# An example of using Python, Seaborn and
# Pandas to plot data into a Boxplot. 

# We will use a library called Seaborn to 
# make our box plot for us, so we import it.
import seaborn as sns
import matplotlib.pyplot as plt

# To graph more than one dataset is a little 
# complicated.  We need to format the
# data into 'dataframes'.  We use the pandas
# library to do this for us.  Import pandas:
import pandas as pd

# Add your measurements into the arrays
# like the following.  
# Here, these are distance between the
# 3Pi+ center and the line center.
x_error_initial = [-9.1, -12.4, -8.6, -10.3, -15.4, -11.2, 10.0, -6.0, -8.1, 1.1]  
x_error_improved = [-12.2, 10.1, 8.1, -8.2, 0.2, 8.5, -6.4, 6.2, 1.1, 0.8]

th_error_initial = [-9.1, -12.4, -8.6, -10.3, -15.4, -11.2, 10.0, -6.0, -8.1, 1.1]  
th_error_improved = [-12.2, 10.1, 8.1, -8.2, 0.2, 8.5, -6.4, 6.2, 1.1, 0.8]

# You can make further arrays, following the
# pattern:
# pwm_60 = [ , , , ]

# We use the .DataFrame() function to apply a 
# label to each array of results
x_initial_dataframe = pd.DataFrame({'X Initial':x_error_initial})
x_improved_dataframe = pd.DataFrame({'X Improved':x_error_improved})

th_initial_dataframe = pd.DataFrame({'Theta Initial':th_error_initial})
th_improved_dataframe = pd.DataFrame({'Theta Improved':th_error_improved})


# We then add the two dataframes together.
x_combined = pd.concat([ x_initial_dataframe, x_improved_dataframe], axis=0)

th_combined = pd.concat([ th_initial_dataframe, th_improved_dataframe], axis=0)

# We can now plot both datasets at the same time.
# plot_x_error = sns.boxplot(data=x_combined)
# plot_x_error.set(xlabel ="Implementation", ylabel = "Travel Error (mm)", title ='Fictional Results')

plot_th_error = sns.boxplot(data=th_combined)
plot_th_error.set(xlabel ="Implementation", ylabel = "Rotation Error (degrees)", title ='Fictional Results')
plt.show()