from flask import Flask, request
import requests

config = None

def majortom():
    print "Got message from Major Tom"
    try:
        headers = {'Content-Type':request.headers['Content-Type']}

        if "telemetry" in request.data:
            DEST = "http://%s:%s" % (config['APP_IP'], config['TELEM_PORT'])
        elif "payload" in request.data:
            DEST = "http://%s:%s" % (config['APP_IP'], config['PAYLOAD_PORT'])

        r = requests.post(DEST, data=request.data, headers=headers, timeout=1)
        return r.text
    except requests.exceptions.RequestException as e:
        print e
        return '{}'

def kubos():
    print "Got message from KubOS"
    try:
        headers = {'Content-Type':'application/json'}
        DEST = "http://%s:%s" % (config['APP_IP'], config['MAJORTOM_PORT'])

        r = requests.post(DEST, data=request.data, headers=headers, timeout=0.1)
        return r.text
    except:
        return '{}'


def create_app(cfg):
    app = Flask(__name__)
    app.debug = True
    global config

    config = cfg

    app.add_url_rule(
        '/majortom',
        view_func=majortom,
        methods=['POST',]
    )

    app.add_url_rule(
        '/kubos',
        view_func=kubos,
        methods=['POST',]
    )

    return app
