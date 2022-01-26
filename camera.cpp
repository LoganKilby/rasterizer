#include "camera.h"

internal void
poll_update_camera_position(preview_context *context, camera *camera)
{
    v3 translation = {};
    f32 step = context->step;
    f32 pan_speed = camera->pan_speed;
    
    if(get_key(context, GLFW_KEY_UP, GLFW_PRESS))
    {
        translation.y += pan_speed * step;
    }
    
    if(get_key(context, GLFW_KEY_DOWN, GLFW_PRESS))
    {
        translation.y -= pan_speed * step;
    }
    
    if(get_key(context, GLFW_KEY_RIGHT, GLFW_PRESS))
    {
        translation.x += pan_speed * step;
    }
    
    if(get_key(context, GLFW_KEY_LEFT, GLFW_PRESS))
    {
        translation.x -= pan_speed * step;
    }
    
    if(get_key(context, GLFW_KEY_W, GLFW_PRESS))
    {
        translation.z += pan_speed * step;
    }
    
    if(get_key(context, GLFW_KEY_S, GLFW_PRESS))
    {
        translation.z -= pan_speed * step;
    }
    
    translation = noz(translation);
    
    camera->origin += translation * pan_speed * step;
    
    if(get_key(context, GLFW_KEY_HOME, GLFW_PRESS))
    {
        camera->rotation.x += camera->turn_speed * step;
    }
    
    if(get_key(context, GLFW_KEY_END, GLFW_PRESS))
    {
        camera->rotation.x -= camera->turn_speed * step;
    }
    
}