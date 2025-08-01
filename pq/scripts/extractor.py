import json 
import re
 
def extract_sensor_data(input_file, output_file):
    # 编译高性能正则表达式（处理嵌套括号）
    pattern = re.compile(r'"sensorData":\s*(\[[^\]]*(?:\[.*?\][^\]]*)*\])') 
    
    with open(input_file, 'r', encoding='utf-8') as infile, \
         open(output_file, 'w', encoding='utf-8') as outfile:
        
        for line_number, line in enumerate(infile, 1):
            line = line.strip() 
            if not line:
                continue
                
            try:
                data = json.loads(line) 
                sensor_content = json.dumps(data.get('sensorData', []), sort_keys = True)
                if(sensor_content != "[]"):
                    outfile.write(sensor_content  + '\n')
                
            except (json.JSONDecodeError, KeyError) as e:
                print(f"解析错误 (行 {line_number}): {str(e)}")
                print(f"问题内容: {line[:50]}...")  # 截断长行 
 
if __name__ == "__main__":
    input_path = "data/rawdata.txt"     # 替换为实际输入文件路径 
    output_path = "data/output.txt"   # 替换为实际输出文件路径 
    
    extract_sensor_data(input_path, output_path)
    print(f"处理完成！结果已保存至: {output_path}")