internal void
move_camera(preview_context *context, v3 *camera_origin, f32 camera_pan_speed)
{
    v3 camera_translation = {};
    
    if(get_key(context, GLFW_KEY_UP, GLFW_PRESS))
    {
        camera_translation.y += camera_pan_speed;
    }
    
    if(get_key(context, GLFW_KEY_DOWN, GLFW_PRESS))
    {
        camera_translation.y -= camera_pan_speed;
    }
    
    if(get_key(context, GLFW_KEY_RIGHT, GLFW_PRESS))
    {
        camera_translation.x += camera_pan_speed;
    }
    
    if(get_key(context, GLFW_KEY_LEFT, GLFW_PRESS))
    {
        camera_translation.x -= camera_pan_speed;
    }
    
    if(get_key(context, GLFW_KEY_W, GLFW_PRESS))
    {
        camera_translation.z += camera_pan_speed;
    }
    
    if(get_key(context, GLFW_KEY_S, GLFW_PRESS))
    {
        camera_translation.z -= camera_pan_speed;
    }
    
    camera_translation = noz(camera_translation);
    
    *camera_origin += camera_translation * camera_pan_speed * context->step;
}