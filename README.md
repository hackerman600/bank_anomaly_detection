# bank anomaly detection
this is an anomaly detection algorithim where no previous fraud data with labels has been recorded. Otherwise I would use a neural network if the data is large enough.

I beleive that using anomaly detection algorithims using data partaining to the particular user is more useful then accessing the data wholistically as each individuals purchasing behavior is different so it is good to compare the recorded activity with their "usual activity". 

for example, 
typical ip location (being one of the features listed below) consists of values between 0 and 1. 0.33 tells me that 33% of the time, the ip addres the particualr user used to make the online purchase was not one linked to Australia.  

I tried to compare as many important features to the average to calculate how much the particular activity deviated from the USERS NORM, NOT THE COLLECTIVE NORM.

IMPORANT - Because I can could not compare "failed log in attempts" and "2fa" with the users "normal activity", I punished the distance score by 2 points for each of the two occuring. I normalised the data and assigned importance scores to the features (I beleived some features should have more sway on the score than others) 

*abs() = absolute difference

algorithim used:
------------------
step 1: calculate the distance score using what I meantioned above: 

formula = 
abs((ip location partaining to the individual user) - (typical ip location partaining to the individual user)) * 1 + 
abs((time of purchase) - (average time of purchase)) * 0.5 + abs(($$ spent) - (average $$ spent)) * 1 + abs((known retailer?) - (known-retailer/unknown ratio %)) * 1 + 
2(if 2fa is enabled) + 2(if recent failed login attempt) +

step 2: calcute the median

step 3: calculate z scores using median absolute deviation and the median rather then the mean

step 4: set thresholds and classify favouring precision and recall using 2 thresholds, send a huge warning to the precision, possibly temp block card until identity is proved using. send minor notification to the recall, possibly get them to prove identity asap.


dataset features
--------------------
- ip location partaining to the individual user (0 = australia, 1 = outside australia)
- typical ip location partaining to the individual user (0-1: 0.3 = 30% outside of australia),
- time of purchase (0.0 - 23.59), 
- average time of purchase (0.0 - 23.59) , 
- $$ spent, 
- average $$ spent, 
- 2FA enabled? (0 = no, 1 = yes), 
- known retailer? (0 = no, 1 = yes), 
- known-retailer/unknown ratio %, 
- failed log in attempts? (0 = no, 1 = yes)

 

