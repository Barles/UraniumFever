#include "../includes/main.h"

int effectVolume = 0;
int musicVolume = MIX_MAX_VOLUME;
int musicHasFinished = 0;
char musicList[256][256];
Mix_Music *music;
Mix_Chunk *whitenoise;

void  pushToMusicList(char *folder, char *file) {
  int i = 0;
  while (strlen(musicList[i]) > 0) {
    i++;
  }

  char *fileName = malloc(sizeof(char) * 256);

  int j = 0;
  while(folder[j] != '\0') {
    fileName[j] = folder[j];
    j++;
  }
  fileName[j++] = '/';
  int h = 0;
  while (file[h] != '\0') {
    fileName[j++] = file[h++];
  }
  fileName[j] = '\0';
  strcpy(musicList[i], fileName);
  free(fileName);
}

int   getMusicListLength() {
  int i = 0;
  while (strlen(musicList[i]) > 0)
    i++;
  return i;
}

void  printMusicList(void) {
  int i = 0;
  while (i < 256 && strlen(musicList[i]) > 0) {
    printf("%d %s\n", i, musicList[i]);
    i++;
  }
}

void  listFilesInFolder(char *folder) {
  DIR *d;
  struct dirent *dir;
  d = opendir(folder);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_type == DT_REG) {
        pushToMusicList(folder, dir->d_name);
      }
    }
    closedir(d);
  }
}

void loadAndPlaySong() {
  int num = rand() % getMusicListLength();
  music = Mix_LoadMUS(musicList[num]);

  if(!music) {
      printf("%s\n", Mix_GetError());
  }
  if(Mix_PlayMusic(music, 1)==-1) {
    printf("Mix_PlayMusic: %s\n", Mix_GetError());
  }

  printf("Music playing => %s (item n°%d/%d)\n", musicList[num], num, getMusicListLength());
}

void  musicFinished(void) {
  Mix_FreeMusic(music);
  loadAndPlaySong();
}

int   main(void) {
  SDL_Event e;

  srand(time(NULL));
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO );
  atexit(SDL_Quit);

  SDL_Window* window = SDL_CreateWindow("DrumPads",
                    SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED,
                    256,
                    256,
                    SDL_WINDOW_RESIZABLE);

  listFilesInFolder("assets");
  printMusicList();

  int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
  if( result < 0 )
  {
      fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
      exit(-1);
  }

  whitenoise = Mix_LoadWAV("assets/effects/whitenoise.wav");
  Mix_Volume(-1, effectVolume);
  if(!whitenoise) {
      printf("Mix_LoadWAV: %s\n", Mix_GetError());
      // handle error
  }
  if(Mix_PlayChannel(-1, whitenoise, 0)==-1) {
    printf("Mix_PlayChannel: %s\n",Mix_GetError());
    // may be critical error, or maybe just no channels were free.
    // you could allocated another channel in that case...
  }

  Mix_VolumeMusic(musicVolume);
  loadAndPlaySong();

  Mix_HookMusicFinished(musicFinished);
  while(1) {
    while(SDL_PollEvent(&e)) {
      if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == 'n') {
          Mix_HaltMusic();
        }
        if (e.key.keysym.sym == 'o') {
          effectVolume -= 1;
          if (effectVolume < 0)
            effectVolume = 0;
          Mix_Volume(-1, effectVolume);
          musicVolume += 16;
          if (musicVolume > MIX_MAX_VOLUME)
            musicVolume = MIX_MAX_VOLUME;
          Mix_VolumeMusic(musicVolume);
        }
        if (e.key.keysym.sym == 'p') {
          effectVolume += 1;
          if (effectVolume > MIX_MAX_VOLUME)
            effectVolume = MIX_MAX_VOLUME;
          Mix_Volume(-1, effectVolume);
          musicVolume -= 16;
          if (musicVolume < 0)
            musicVolume = 0;
          Mix_VolumeMusic(musicVolume);
        }
      } else if (e.type == SDL_QUIT) {
        musicHasFinished = 1;
      }
    }
    if (musicHasFinished == 1) {
      printf("Goodbye.\n");
      break;
    }
  }

  return EXIT_SUCCESS;
}
