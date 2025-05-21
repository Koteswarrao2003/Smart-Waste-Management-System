from flask import Flask, render_template, jsonify
from influxdb_client import InfluxDBClient

app = Flask(__name__, static_folder="static")

# InfluxDB credentials
url = "http://127.0.0.1:8086"
token = "XWtRc64Cw6jEXYPy8cbArHALfzvuNb5Kj1zJL4zkl5DdmFySFM6DaiSiXU0m0PFwSBoPf7p4sbNIwiRKZGiRig=="
org = "Koti_Huebits"
bucket = "Koti_Huebits_Buck"

client = InfluxDBClient(url=url, token=token, org=org)


@app.route('/map')
def map_view():
    return render_template('map2.html')


@app.route('/analysis')
def analysis_view():
    return render_template('analysis.html')


@app.route('/api/data')
def api_data():
    query_api = client.query_api()

    query = f'''
    from(bucket: "{bucket}")
      |> range(start: -1h)
      |> filter(fn: (r) => r._measurement == "mqtt_consumer")
      |> filter(fn: (r) => r._field == "distance")
      |> filter(fn: (r) => r.host == "Koteswara-Rao")
      |> filter(fn: (r) => r.topic == "esp32")
      |> sort(columns: ["_time"])
    '''

    result = query_api.query(org=org, query=query)
    distances = []
    timestamps = []

    for table in result:
        for record in table.records:
            distances.append(record.get_value())
            timestamps.append(record.get_time().strftime('%H:%M:%S'))

    waste_level = "No Data"
    if distances:
        latest = distances[-1]
        if latest <= 10:
            waste_level = "High"
        elif latest <= 20:
            waste_level = "Medium"
        else:
            waste_level = "Low"

    return jsonify({
        'timestamps': timestamps,
        'distances': distances,
        'waste_level': waste_level
    })


@app.route('/api/waste_level')
def get_waste_level():
    query_api = client.query_api()

    query = f'''
    from(bucket: "{bucket}")
      |> range(start: -1h)
      |> filter(fn: (r) => r._measurement == "mqtt_consumer")
      |> filter(fn: (r) => r._field == "distance")
      |> filter(fn: (r) => r.host == "Koteswara-Rao")
      |> filter(fn: (r) => r.topic == "esp32")
      |> sort(columns: ["_time"], desc: true)
      |> limit(n: 1)
    '''

    result = query_api.query(org=org, query=query)
    values = [record.get_value() for table in result for record in table.records]

    waste_level = "No Data"
    if values:
        latest_distance = values[0]
        if latest_distance <= 10:
            waste_level = "High"
        elif latest_distance <= 20:
            waste_level = "Medium"
        else:
            waste_level = "Low"

    return jsonify({"waste_level": waste_level})


if __name__ == '__main__':
    app.run(debug=True)
