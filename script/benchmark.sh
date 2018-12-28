#!/bin/sh

DATASET_DIR=
INPUT_IMAGE_DIR=../dataset/image
INPUT_KEYPOINT_DIR=../dataset/keypoints
INPUT_GROUNDTRUTH_DIR=../dataset/gt
OUTPUT_BENCHMARK_DIR=../output/benchmark
OUTPUT_MASK_DIR=../output/mask
OUTPUT_FIGURE_DIR=../output/figure
OUTPUT_MARKED_DIR=../output/marked

mkdir -p ${OUTPUT_MASK_DIR}
mkdir -p ${OUTPUT_BENCHMARK_DIR}
mkdir -p ${OUTPUT_FIGURE_DIR}
mkdir -p ${OUTPUT_MARKED_DIR}

for i in $(seq 1 23)
do
    echo $i
    ./segment ${INPUT_IMAGE_DIR}/${i}.jpeg \
              ${INPUT_KEYPOINT_DIR}/std_keypoint${i}.csv \
              ${OUTPUT_MASK_DIR}/mask${i}.png \
              ${OUTPUT_FIGURE_DIR}/figure${i}.png \
              ${OUTPUT_MARKED_DIR}/marked${i}.png \
              ${INPUT_GROUNDTRUTH_DIR}/gt${i}.PNG \
              ${OUTPUT_BENCHMARK_DIR}/bk${i}.csv
    # ./segment ../dataset/4.jpeg ../dataset/kp4.csv ../output/mask-4.png ../dataset/gt4.PNG ../output/benchmark4.csv
done

head -n 2 ${OUTPUT_BENCHMARK_DIR}/bk1.csv >${OUTPUT_BENCHMARK_DIR}/bk.csv
for i in $(seq 2 23)
do
    head -n 2 ${OUTPUT_BENCHMARK_DIR}/bk${i}.csv | tail -n 1 >>${OUTPUT_BENCHMARK_DIR}/bk.csv
done
