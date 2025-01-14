import re
import sys

def assign_enum_values(read_proto_file_path, write_proto_file_path):
    with open(read_proto_file_path, 'r') as file:
        lines = file.readlines()

    enum_pattern = re.compile(r'\benum\b\s+\w+\s*')
    constant_pattern = re.compile(r'^\s*(\w+)\s*;')
    in_enum = False
    value_counter = 0

    for i, line in enumerate(lines):
        if enum_pattern.search(line):
            in_enum = True
            value_counter = 0
        elif in_enum:
            if '}' in line:  # Enum block 종료
                in_enum = False
            elif constant_pattern.match(line):  # Enum 상수 감지
                match = constant_pattern.match(line)
                if match:
                    constant_name = match.group(1)
                    # 숫자 값 추가
                    lines[i] = line.replace(
                        f"{constant_name};", f"{constant_name} = {value_counter};"
                    )
                    value_counter += 1

    # 수정된 파일 덮어쓰기
    with open(write_proto_file_path, 'w') as file:
        file.writelines(lines)

# 사용법
readFilePath = sys.argv[1]
writeFilePath = sys.argv[2]

assign_enum_values(readFilePath, writeFilePath)  # 프로토 파일 경로 지정