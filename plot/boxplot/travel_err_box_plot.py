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
pwm_20 = [-9.1, -12.4, -8.6, -10.3, -15.4, -11.2, 10.0, -6.0, -8.1, 1.1]  
pwm_40 = [-12.2, 10.1, 8.1, -8.2, 0.2, 8.5, -6.4, 6.2, 1.1, 0.8]

# You can make further arrays, following the
# pattern:
# pwm_60 = [ , , , ]

# We use the .DataFrame() function to apply a 
# label to each array of results
pwm_20_dataframe = pd.DataFrame({'20':pwm_20})
pwm_40_dataframe = pd.DataFrame({'40':pwm_40})
# pwm_60_dataframe = pd.DataFrame({'60':pwm_60})

# We then add the two dataframes together.
combined = pd.concat([ pwm_20_dataframe, pwm_40_dataframe], axis=0)
# combined = pd.concat([ pwm_20_dataframe, pwm_40_dataframe, pwm_60_dataframe], axis=0)

# We can now plot both datasets at the same time.
# Note, data=combined!

plot = sns.boxplot(data=combined)
plot.set(xlabel ="PWM Value", ylabel = "Travel Error (mm)", title ='Fictional Results')
# ax.annotate(    
#     text='Fastest Car',
#     xy=(20, -8),
#     xycoords='data',
#     fontsize=20,
#     xytext=(0.8, 0.9),
#     arrowprops=dict(arrowstyle='->', color='black'),  # Use color black
#     horizontalalignment='center',  # Center horizontally
#     verticalalignment='center')  # Center vertically)
plt.show()