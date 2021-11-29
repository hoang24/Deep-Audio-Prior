import soundfile as sf
import numpy as np
from scipy.io import wavfile
import librosa
import os
import subprocess
import noisereduce

audLen = 8 # length of trimmed recording (seconds)

cosep_root = '/content/drive/MyDrive/EE/EE225D_Project/mri'

input_dir = cosep_root
output_dir = cosep_root + '_preprocessed'

sub_list = os.listdir(input_dir)

for folder in sub_list:
    if 'sub' not in folder:
        continue

    try:
        os.makedirs(output_dir + "/" + folder)
    except FileExistsError:
        continue

    video_input_dir = input_dir + "/" + folder + "/"
    dir_list = os.listdir(video_input_dir)

    for video_file in dir_list:
        video_title = video_file.split('.')[0]
        y, sr = librosa.load(input_dir + "/" + folder + "/" + video_file, mono=False)
        N_trim = audLen * sr;
        audio = y[:, 0:N_trim]
        sf.write(output_dir + "/" + folder + "/" + video_title + f'_first{audLen}sec.wav', np.transpose(audio), sr)

    print("Completed " + folder)
