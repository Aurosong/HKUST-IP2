import geopandas as gpd 
import matplotlib.pyplot  as plt
import time
from sqlalchemy import create_engine
 
# 连接数据库 
engine = create_engine('postgresql://chen@localhost/ip2')

start = time.perf_counter_ns()

gdf = gpd.read_postgis( 
    "SELECT id, geom FROM jsontb where id = 1", 
    engine,
    geom_col='geom'
)

elapsed = (time.perf_counter_ns() - start) / 1e6

print(f"执行耗时: {elapsed:.2f} ms")
 
# 可视化 
ax = gdf.plot(figsize=(15,10),  color='blue', linewidth=1.5)
ax.set_title(" 交通网络空间分布", fontsize=18)
ax.set_axis_off() 
plt.savefig('data/network.png',  dpi=300)