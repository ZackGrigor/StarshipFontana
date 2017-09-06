#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);


  //number of aliens that are in the game and where they are positioned
  const int number_of_aliens = 2;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2((canvas_w/number_of_aliens  ) * i+100, 400.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

 //number of bricks that are in the game and where they are positioned on the window
  const int number_of_brick = 6;
  for (int i = 0; i < number_of_brick; i++){
	  auto brick = make_shared<SFAsset>(SFASSET_BRICK, sf_window);
	  auto pos = Point2((canvas_w/number_of_brick) * i+50, 300.0f);
	  brick->SetPosition(pos);
	  bricks.push_back(brick);
  }

  //where the coin is positioned on the screen
  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2((canvas_w/5), 450);
  coin->SetPosition(pos);
  coins.push_back(coin);
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_PLAYER_DOWN:
	  player->GoSouth();
	  break;
  case SFEVENT_PLAYER_UP:
      player->GoNorth();
      break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();

  }


  	for(auto c: coins){
  	}


  // The aliens will go south
  for(auto a : aliens) {
	  a->GoSouth();
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
      }
    }
  }


  //Collision of player and aliens, which will mean when they collide the game is over.
  for(auto a: aliens){
    	  if(player->CollidesWith(a)){
    		  player->SetNotAlive();
    		  	player->HandleCollision();
    		 	a->HandleCollision();
    		  	  	cout << "Gameover" << endl;
    		  	    	}
      					}

  //Collision of the player and the bricks, which will mean when they collide the game is classed as over
  for(auto b: bricks){
	  if (player->CollidesWith(b)){
		  	  player->SetNotAlive();
				cout <<"Gameover" << endl;
						  }
  	  	  	  	  	  	  	  }

  // Collision of the player and the coin, which allows the player to win and will display in console
  for (auto c: coins){
	  if (player->CollidesWith(c)){
		  c->HandleCollision();
		  player->HandleCollision();
		  coins.clear();
		  cout << "You win" << endl;
	  }
  }
  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
 for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
		}



void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  player->OnRender();

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    c->OnRender();
  }
  for (auto b: bricks){
	b->OnRender();
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
