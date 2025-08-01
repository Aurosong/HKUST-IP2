import json
import matplotlib.pyplot  as plt
from shapely.geometry  import LineString, MultiLineString
import seaborn as sns  # 新增seaborn导入 
 

def load_geospatial_data(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f) 
    
    if not data.get('features'): 
        raise ValueError("无效的GeoJSON格式：缺少features字段")
    
    return data
 
def convert_to_linestrings(data):
    linestrings = []
    
    for feature in data['features']:
        geom = feature['geometry']
        if geom['type'] == 'MultiLineString':
            for line_coords in geom['coordinates']:
                points_2d = [[x, y] for x, y, *_ in line_coords]
                linestrings.append(LineString(points_2d)) 
    
    return MultiLineString(linestrings)

 
def visualize_linestrings(linestring_data):
    plt.figure(figsize=(14,  10), dpi=120)
    ax = plt.subplot(111) 
    
    sns.set_style("whitegrid")
    ax.set_facecolor('#f0f8ff') 
    
    for i, line in enumerate(linestring_data.geoms): 
        x, y = line.xy  
        plt.plot(x,  y, 
                 linewidth=1.5,
                 alpha=0.7,
                 marker='.',
                 markersize=4,
                 color=plt.cm.tab20(i  % 20))
        
        if i < 5:
            for j, (point_x, point_y) in enumerate(zip(x, y)):
                if j % 10 == 0:
                    plt.text(point_x,  point_y, 
                             f'P{i+1}-{j}',
                             fontsize=6,
                             alpha=0.8)
    plt.savefig('data/network.png',  dpi=300)
 

if __name__ == "__main__":
    FILE_PATH = 'data/map_sample.json' 
    
    try:
        raw_data = load_geospatial_data(FILE_PATH)
        linestrings = convert_to_linestrings(raw_data)
        
        print(f"成功转换 {len(linestrings.geoms)}  条LineString路径")
        print(f"空间覆盖范围: {linestrings.bounds}") 
        
        visualize_linestrings(linestrings)
        
    except Exception as e:
        print(f"处理错误: {str(e)}")