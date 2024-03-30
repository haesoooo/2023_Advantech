import pandas as pd
import numpy as np
from sklearn.tree import DecisionTreeClassifier, export_graphviz
import graphviz
from sklearn.model_selection import train_test_split
from sklearn.impute import SimpleImputer
from sklearn.pipeline import Pipeline
from sklearn.metrics import accuracy_score
from sklearn.ensemble import HistGradientBoostingClassifier
from IPython.display import Image
import pymysql
import pydot
import pygraphviz

file_path = '/home/mic-711on/Downloads/samplecsvdata.csv'
df = pd.read_csv(file_path, skiprows=[0])

df['적절성'] = ((df['내부온도(도)'].between(25, 31)) &
               (df['내부습도(%)'].between(60, 80)) &
               (df['일사량(W/m-2*s)(외부)'].between(500, 1000))).astype(int)

imputer = SimpleImputer(strategy='mean')
X = imputer.fit_transform(df[['내부온도(도)', '내부습도(%)', '일사량(W/m-
2*s)(외부)']])
y = df['적절성']

model = DecisionTreeClassifier(random_state=42)
model.fit(X, y)
db = pymysql.connect(host='127.0.0.1', port=3306, user='test', password 
= '1111', database = 'data')
cursor = db.cursor()
query = "SELECT TEMPERATURE, HUMIDITY, LIGHT FROM real_data ORDER BY id 
DESC LIMIT 1;"
cursor.execute(query)
row = cursor.fetchone()
new_data = [row[0], row[1], row[2]]
print("\n---Recent Environment Data---")
query = "SELECT data_format(DATE_TIME, '%Y-%m-%d %H: %i: %s') from 
real_data order by id desc limit 1;"
cursor.execute(query)
times = cursor.fetchone()
print("\nData Collecting Time: ", times)
print("\n Temperature : ", row[0], "℃")
print("\n Humidity : ", row[1], " %")
print("\n Light : ", row[2], " lux")

print("---------------------------\n")
new_data_arr = np.array(new_data)
new_data_2d = new_data_arr.reshape(1, -1)

prediction = model.predict(new_data_2d)
dot_data = export_graphviz(model,
                           out_file=None,
                           feature_names=['Temperature', 'Humidity', 
'Illuminance'],
                           class_names=['Inappropriate', 'Appropriate'],
                           filled=True,
                           rounded=True,
                           special_characters=True)

graph = graphviz.Source(dot_data)
graph = graphviz.Source(dot_data)
graph.render("decision_tree", directory= './', format="png", 
cleanup=True)

if prediction == 0:
  is_inappropriate_temperature = not (20 <= row[0] <= 31)
  is_inappropriate_illuminance = not (500 <= row[2] <= 1000)

  if is_inappropriate_temperature and is_inappropriate_illuminance:
    db = pymysql.connect(host='127.0.0.1', port=3306, user='test', 
password = '1111', database = 'data')
    cursor = db.cursor()
    query = "INSERT INTO judge VALUES(NULL, 'Inappropriate Temperature', 
'Inappropriate Light');"
    cursor.execute(query)
    db.commit()
    db.close()
    print("inappropriate temperature & light !! 조도 및 온도 제어가 필요 
합니다.\n")
  elif is_inappropriate_illuminance:
    db = pymysql.connect(host='127.0.0.1', port=3306, user='test', 
password = '1111', database = 'data')
    cursor = db.cursor()
    query = "INSERT INTO judge VALUES(NULL, NULL, 'Inappropriate 
Light');"
    cursor.execute(query)
    db.commit()
    db.close()
    print("inappropriate light !! 조도 제어가 필요 합니다.\n")

  elif is_inappropriate_temperature:
    db = pymysql.connect(host='127.0.0.1', port=3306, user='test', 
password = '1111', database = 'data')
    cursor = db.cursor()
    query = "INSERT INTO judge VALUES(NULL, 'Inappropriate Temperature', 
NULL);"
    cursor.execute(query)
    db.commit()
    db.close()
    print("inappropriate temperature !! 온도 제어가 필요 합니다.\n")
    



