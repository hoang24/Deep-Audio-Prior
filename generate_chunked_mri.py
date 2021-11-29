import soundfile as sf
import numpy as np
import librosa
import os
import subprocess
import noisereduce

cosep_root = 'mri'

input_dir = cosep_root
output_dir = cosep_root + '_preprocessed'

sub_list = os.listdir(input_dir)

for folder in sub_list:
    if not 'sub' in folder:
        break
    os.makedirs(output_dir + "/" + folder)
    video_input_dir = input_dir + "/" + folder + "/"
    dir_list = os.listdir(video_input_dir)

    for video_file in dir_list:
        video_title = video_file.split('.')[0]
        os.makedirs(output_dir + "/" + folder + "/" + video_title)
        y, sr = librosa.load(input_dir + "/" + folder + "/" + video_file, mono=False)
        y_reduced = noisereduce.reduce_noise(y=y, sr=sr)
        y_extended = np.pad(y_reduced, (0, sr*5 - len(y_reduced)%(sr*5)), 'constant')
        y_split = np.reshape(y_extended, (-1, 5*sr))
        i = 0
        for chunk in y_split:
            i += 1
            print(folder + video_title + i)
            sf.write(output_dir + "/" + folder + "/" + video_title + '_chunk' + i + '.wav', chunk, sr)
        #command = "ffmpeg -i " + video_input_dir + video_title + ".mp4 -ab 160k -ac 2 -ar 44100 -vn " + output_dir + "/" + folder + "/" + video_title + ".wav"
        
        # subprocess.call(command, shell=True)
    print("Completed " + folder)