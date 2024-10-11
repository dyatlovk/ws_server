lua << EOF
  local nvim_lsp = require('lspconfig')
  nvim_lsp.clangd.setup {}
  nvim_lsp.cmake.setup {}

  local conform = require("conform")
  conform.formatters_by_ft.cpp = {{"clangd"}}
  conform.formatters_by_ft.cmake = {{"cmake"}}

  local map = vim.keymap.set
  map("n", "<F12>", "<cmd>ClangdSwitchSourceHeader<CR>", { desc = "SwitchSourceHeader" })
EOF

autocmd BufNewFile,BufRead *.h++ setfiletype cpp
