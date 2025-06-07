import subprocess
import random

blender_path = "../blender-2.79-linux-glibc219-x86_64/blender"

if __name__ == "__main__":
    num = 10
    # max_length = 15
    # min_length = 4
    for i in range(num):
        # 生成多相图片的相数目
        length = 4
        print(f"正在生成第{i+1}组图片，图片序列长度为{length+2}")
        # 使用subprocess.run()函数执行文件
        try:
            result = subprocess.run(
                [
                    blender_path,
                    "--background",
                    "--python",
                    "new_render_12_with5changes.py",
                    "--",
                    "--num_images",
                    "1",
                    "--use_gpu",
                    "1",
                    "--min_objects",
                    "4",
                    "--max_objects",
                    "6",
                    "--output_image_dir",
                    "../test_output/images/",
                    "--output_scene_dir",
                    "../test_output/scenes/",
                    "--start_idx",
                    str(i),
                    "--min_change",
                    "0",
                    "--max_change",
                    str(length),
                ],
                check=True,
            )
        except subprocess.CalledProcessError as e:
            print(f"执行失败，返回码：{e.returncode}")
