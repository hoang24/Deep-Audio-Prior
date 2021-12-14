import librosa
import os
import numpy as np


input_file = 'sub041_2drt_01_vcv1_r2_video_first8sec'
y, sr = librosa.load(input_file + '.wav', mono=False)
np.save(input_file + '.dat', y)

# sf.write(output_dir + "/" + folder + "/" + video_title + f'_first{audLen}sec.wav', np.transpose(audio), sr)
