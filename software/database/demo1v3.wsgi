#!/usr/bin/python
# -*- coding: utf-8 -*-

import time
import sqlite3
from cgi import parse_qs, escape
import gviz_api
import pdb

html = \
    """
<html><body><b>Demo1</b><br>
%(datas)s
</body></html>
"""

   
# r,g,b

#conn = sqlite3.connect('submit2.db')
#c = conn.cursor()
#%(json_str)s<br>
def application(environ, start_response):
    params = parse_qs(environ['QUERY_STRING'])

    conn = sqlite3.connect('/var/www/rover1/wsgi/db/submit2.db')
    c = conn.cursor()

    rows = c.execute('SELECT * FROM data1 ORDER BY timecode')
    datas = ''
    #for row in rows:
    #    datas += (str(row)+'<br>')
    for row in rows:
        datas+='<tr>'
        for col in row:
            datas+=('<td>'+str(col)+'</td>')
        datas+='</tr>'
    datas+='</table>'

    #conn.close()
  #  output = html % {
  #      'datas' : datas,
  #      }

    rows2 = c.execute('SELECT * FROM data1 ORDER BY timecode')

    data=[]
    
    for row2 in rows2:
        
        data.append((row2[3],row2[0],row2[1],row2[2]))
        
        
    output2 = graph(data)

 #  output2 = output2.encode('utf-8')

    status = '200 OK'

    # output = b'Hello World!'

    response_headers = [('Content-type', 'text/html'), ('Content-Length'
                        , str(len(output2)))]
    start_response(status, response_headers)

    return [output2]

def graph(data):
    description=[("Timecode","number"),("Red","number"),("Green","number"),("Blue","number")]
    
    #create a DataTable object
    data_table = gviz_api.DataTable(description)
    data_table.LoadData(data)

    #convert to JSON (could also send no parameters, this just demonstrates that you can re-order
    json_str=data_table.ToJSon(columns_order=("Timecode","Red","Green","Blue"))
#hAxis: {title: 'Timecode in seconds'}
    bar_template = """
    <html>
      <head>
        <script type="text/javascript" src="https://www.google.com/jsapi"></script>
        <script type="text/javascript">
          google.load("visualization", "1", {packages:["table","corechart"]});
          google.setOnLoadCallback(drawChart);
          function drawChart() {
            var data = new google.visualization.DataTable(%(json_str)s);
            var options = {
              
              title: 'AggiE RGB Experiment',
              
              vAxis: {title: 'Color Intensity', viewWindow: { min:0 }},
              hAxis: {title: 'time',viewWindow: { min:1477150000 , max: 1477160000 }}	

          
            };
            var chart = new google.visualization.ScatterChart(document.getElementById('chart_div'));
            chart.draw(data, options);
          

		
		var json_table = new google.visualization.Table(document.getElementById('table_div_json'));
      		json_table.draw(data, {showRowNumber: true});
}	
        </script>
      </head>
      <body>
     	<div id="table_div_json"></div> 
        <div id="chart_div" style="width: 900px; height: 500px;"></div>
      </body>
    </html>
    """ % {
            'json_str' : json_str,
        }
    return bar_template
