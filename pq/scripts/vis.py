import numpy as np
import pandas as pd 
import plotly.express  as px 
import webbrowser

field = "rotationRate"
 
# 加载数据
df = pd.read_csv(f'../data/csv/{field}_samples.csv') 
 
# 创建交互式3D散点图 
fig = px.scatter_3d( 
    df, 
    x='x', y='y', z='z',
    color='vector_magnitude',
    color_continuous_scale='Viridis',
    size='vector_magnitude',
    size_max=15,
    opacity=0.7,
    title='三维向量空间分布',
    labels={'vector_magnitude': '向量模长'}
)
 
# 增强可视化效果
fig.update_layout( 
    scene=dict(
        xaxis=dict(backgroundcolor="rgba(0,0,0,0)", gridcolor="lightgray"),
        yaxis=dict(backgroundcolor="rgba(0,0,0,0)", gridcolor="lightgray"),
        zaxis=dict(backgroundcolor="rgba(0,0,0,0)", gridcolor="lightgray"),
        bgcolor='white'
    ),
    hovermode='closest',
    height=800 
)
 
# 添加向量方向指示（箭头）
# for _, row in df.sample(100).iterrows():   # 随机采样100个向量显示方向 
#     fig.add_trace(px.line_3d( 
#         pd.DataFrame({
#             'x': [0, row['x']],
#             'y': [0, row['y']],
#             'z': [0, row['z']]
#         }), 
#         x='x', y='y', z='z'
#     ).data[0].update(line=dict(color='red', width=2)))
 
fig.write_html(f"../data/html/{field}.html") 