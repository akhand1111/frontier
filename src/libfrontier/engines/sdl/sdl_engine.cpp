
#include "sdl_engine.h"

using namespace Geek;

FrontierEngineSDL::FrontierEngineSDL(FrontierApp* app) : FrontierEngine(app)
{
}

FrontierEngineSDL::~FrontierEngineSDL()
{
}


bool FrontierEngineSDL::init()
{
    int res;
    res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (res < 0)
    {
        log(ERROR, "Failed to initialise SDL!");
    }

    return true;
}

bool FrontierEngineSDL::initWindow(FrontierWindow* window)
{
    bool res;

    FrontierEngineWindowSDL* few = new FrontierEngineWindowSDL(window);

    res = few->init();
    if (!res)
    {
        return false;
    }

    window->setEngineWindow(few);

    return true;
}

static FrontierEngineWindowSDL* getWindow(uint32_t id)
{
    SDL_Window* window = SDL_GetWindowFromID(id);
    if (window == NULL)
    {
        return NULL;
    }

    return (FrontierEngineWindowSDL*)SDL_GetWindowData(window, "FrontierWindow");
}

bool FrontierEngineSDL::checkEvents()
{
    SDL_Event event;
    SDL_WaitEvent(&event);

    switch (event.type)
    {
        case SDL_QUIT:
        {
            return false;
        } break;

            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
            {
                FrontierEngineWindowSDL* few = getWindow(event.button.windowID);
                if (few == NULL)
                {
                    return true;
                }

                InputMessage* msg = new InputMessage();
                msg->messageType = FRONTIER_MSG_INPUT;
                msg->inputMessageType = FRONTIER_MSG_INPUT_MOUSE_BUTTON;
                msg->event.button.direction =
                    (event.button.type == SDL_MOUSEBUTTONDOWN);
                msg->event.button.button = event.button.button;
                msg->event.button.doubleClick = (event.button.clicks == 2);


/*
                if (m_highDPI)
                {
                    int screenw;
                    int screenh;
                    SDL_GetWindowSize(m_screen, &screenw, &screenh);
                    int draww;
                    int drawh;
                    SDL_GL_GetDrawableSize(m_screen, &draww, &drawh);
                    msg->event.button.x = (int)((((float)event.button.x / (float)screenw)) * (float)draww);
                    msg->event.button.y = (int)((((float)event.button.y / (float)screenh)) * (float)drawh);
                }
                else
*/
                {
                    msg->event.button.x = event.button.x;
                    msg->event.button.y = event.button.y;
                }
printf("FrontierEngineSDL::checkEvents: few=%p, x=%d, y=%d\n", few, msg->event.button.x, msg->event.button.y);
few->getWindow()->handleMessage(msg);
} break;

            case SDL_MOUSEMOTION:
            {
                FrontierEngineWindowSDL* few = getWindow(event.button.windowID);
                if (few == NULL)
                {
                    return true;
                }

                // Limit motion events to one per frame
/*
                uint64_t t = SDL_GetTicks();
                if ((t - m_lastMotion) < (1000 / 60))
                {
                    continue;
                }
                m_lastMotion = t;
*/

                InputMessage* msg = new InputMessage();
                msg->messageType = FRONTIER_MSG_INPUT;
                msg->inputMessageType = FRONTIER_MSG_INPUT_MOUSE_MOTION;
/*
                if (m_highDPI)
                {
                    int screenw;
                    int screenh;
                    SDL_GetWindowSize(m_screen, &screenw, &screenh);
                    int draww;
                    int drawh;
                    SDL_GL_GetDrawableSize(m_screen, &draww, &drawh);
                    msg->event.motion.x = (int)((((float)event.motion.x / (float)screenw)) * (float)draww);
                    msg->event.motion.y = (int)((((float)event.motion.y / (float)screenh)) * (float)drawh);
                }
                else
*/
                {
                    msg->event.button.x = event.button.x;
                    msg->event.button.y = event.button.y;
                }
few->getWindow()->handleMessage(msg);
} break;

    }

    return true;
}

