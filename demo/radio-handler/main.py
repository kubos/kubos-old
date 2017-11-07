import argparse
from app import *
import yaml

parser = argparse.ArgumentParser(description='Radio demo')
parser.add_argument('config', type=str, help='path to config file')
args = parser.parse_args()

with open(args.config) as ymlfile:
    cfg = yaml.load(ymlfile)


app = create_app(cfg)
app.run(host=cfg['APP_IP'], port=cfg['RADIO_PORT'])
