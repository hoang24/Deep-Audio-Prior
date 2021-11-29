import subprocess
import os

input_dir = "../../2drt_speaker"
output_dir = "data/cosep/mri"

sub_list = os.listdir(input_dir)

for folder in sub_list:
    if not 'sub' in folder:
        break
    os.makedirs(output_dir + "/" + folder)
    video_input_dir = input_dir + "/" + folder + "/2drt/video/"
    dir_list = os.listdir(video_input_dir)

    for video_file in dir_list:
        video_title = video_file.split('.')[0]
        
        command = "ffmpeg -i " + video_input_dir + video_title + ".mp4 -ab 160k -ac 2 -ar 44100 -vn " + output_dir + "/" + folder + "/" + video_title + ".wav"

        subprocess.call(command, shell=True)