from flask import Flask, request
import requests

app = Flask(__name__)
app.debug = True

APP_IP = "http://127.0.0.1"
TELEMETRY = "%s:5001/graphql" % APP_IP
PAYLOAD = "%s:5002" % APP_IP
MAJORTOM = "%s:5003" % APP_IP


@app.route('/majortom', methods=['POST'])
def majortom():
    print "Got message from Major Tom"
    print request.data
    try:
        headers = {'Content-Type':request.headers['Content-Type']}

        if "telemetry" in request.data:
            DEST = TELEMETRY
        elif "payload" in request.data:
            DEST = PAYLOAD

        r = requests.post(DEST, data=request.data, headers=headers, timeout=0.1)
        return r.text
    except:
        return '{}'

@app.route('/kubos', methods=['POST'])
def kubos():
    print "Got message from KubOS"
    try:
        headers = {'Content-Type':'application/json'}
        r = requests.post(MAJORTOM, data=request.data, headers=headers, timeout=0.1)
        return r.text
    except:
        return '{}'

@app.teardown_appcontext
def shutdown_session(exception=None):
    pass


if __name__ == '__main__':
    app.run()
