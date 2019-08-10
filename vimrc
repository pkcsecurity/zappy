" Load Vim 8 defaults
unlet! skip_defaults_vim
source $VIMRUNTIME/defaults.vim
" defaults.vim sets this to 5 but I don't want it
set scrolloff=0

let mapleader=','
set laststatus=2 " enable status line on all windows
set hidden " allow edited buffers to be hidden
set number " show line numbers
set cursorline " highlight the current line
set lazyredraw
set encoding=utf-8
set foldlevelstart=99
nnoremap j gj
nnoremap k gk

call plug#begin()
Plug '~/prog/arduino/zappy/vim_plugin'
call plug#end()

augroup UpdateCPMGroup
    au!
    autocmd InsertCharPre * :UpdateWPM
augroup END
