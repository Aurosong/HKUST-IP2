import psycopg2 
from psycopg2.extras import execute_batch 
import matplotlib.pyplot  as plt
from shapely.geometry  import LineString, MultiLineString 
import numpy as np
import config 
import json
from psycopg2 import sql 
import argparse 
import math

def convert2linestrings(file_path):
    # load data
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f) 

    # convert data
    linestrings = []
    for feature in data['features']:
        geom = feature['geometry']
        if geom['type'] == 'MultiLineString':
            for line_coords in geom['coordinates']:
                points_2d = [[x, y] for x, y, *_ in line_coords]
                linestrings.append(LineString(points_2d)) 

    converted_data = {'id': data['features'][0]['properties']['OBJECTID'], 'geom': MultiLineString(linestrings)}
    return converted_data
    

def setup_database(data, file_path):

    with open(file_path, 'r', encoding='utf-8') as f:
        jsondata = json.load(f) 

    print("Setting up database...")

    conn = psycopg2.connect(**config.DB_CONFIG) 
    cur = conn.cursor() 
     
    cur.execute("CREATE EXTENSION IF NOT EXISTS postgis;")
    table_name = 'test'

    cur.execute(f""" 
        CREATE TABLE {table_name} (
            id SERIAL PRIMARY KEY,
            geom GEOMETRY(MULTILINESTRING, 4326),
        );
    """)

    insert_sql = sql.SQL(f"""
        INSERT INTO {table_name} (id, geom, json)
        VALUES (%s, ST_GeomFromText(%s, 4326))
    """)

    cur.execute(insert_sql, (data['id'], data['geom'].wkt, json.dumps(jsondata)))

    conn.commit()
    cur.close() 
    conn.close() 

    print("Database setup complete.")


if __name__ == "__main__":
    file_path = 'data/map_sample.json'

    data = convert2linestrings(file_path)
    setup_database(data, file_path)