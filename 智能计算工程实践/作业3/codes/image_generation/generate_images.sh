for k in $(seq 0 60000)
do
  ../blender-2.79-linux-glibc219-x86_64/blender --background --python render_12_with5changes.py -- --num_images 1 --use_gpu 1 --min_objects 4 --max_objects 6 --output_image_dir '../output/images/' --output_scene_dir '../output/scenes' --start_idx $k
done
