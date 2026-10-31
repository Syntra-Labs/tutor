local M = {}

local tutor_bin = nil

local function get_tutor_bin()
    if tutor_bin then return tutor_bin end
    local env_dir = os.getenv("TUTOR_REPO_DIR")
    if env_dir and vim.fn.filereadable(env_dir .. "/bin/tutor") == 1 then
        tutor_bin = env_dir .. "/bin/tutor"
        return tutor_bin
    end
    local local_bin = vim.fn.expand("~/.local/bin/tutor")
    if vim.fn.filereadable(local_bin) == 1 then
        tutor_bin = local_bin
        return tutor_bin
    end
    tutor_bin = "tutor"
    return tutor_bin
end

--- Open a floating terminal window running a tutor command
local function run_in_float(cmd_str, title)
    local width = math.floor(vim.o.columns * 0.85)
    local height = math.floor(vim.o.lines * 0.8)
    local row = math.floor((vim.o.lines - height) / 2)
    local col = math.floor((vim.o.columns - width) / 2)

    local buf = vim.api.nvim_create_buf(false, true)
    local win = vim.api.nvim_open_win(buf, true, {
        relative = "editor",
        width = width,
        height = height,
        row = row,
        col = col,
        style = "minimal",
        border = "rounded",
        title = " " .. (title or "Tutor") .. " ",
        title_pos = "center",
    })

    vim.fn.termopen(cmd_str, {
        on_exit = function()
            vim.api.nvim_buf_set_keymap(buf, "n", "q", "<cmd>close<CR>", { noremap = true, silent = true })
            vim.api.nvim_buf_set_keymap(buf, "n", "<Esc>", "<cmd>close<CR>", { noremap = true, silent = true })
        end,
    })
    vim.cmd("startinsert")
end

--- Interactive dashboard buffer listing all problems
function M.dashboard()
    local bin = get_tutor_bin()
    local output = vim.fn.system(bin .. " list")

    local buf = vim.api.nvim_create_buf(false, true)
    vim.api.nvim_buf_set_name(buf, "Tutor Dashboard")
    vim.api.nvim_set_option_value("buftype", "nofile", { buf = buf })
    vim.api.nvim_set_option_value("filetype", "tutor", { buf = buf })

    local lines = vim.split(output, "\n")
    vim.api.nvim_buf_set_lines(buf, 0, -1, false, lines)
    vim.api.nvim_set_current_buf(buf)

    -- Keybindings inside dashboard
    vim.api.nvim_buf_set_keymap(buf, "n", "<CR>", "<cmd>lua require('tutor').pick_under_cursor()<CR>", { noremap = true, silent = true })
    vim.api.nvim_buf_set_keymap(buf, "n", "p", "<cmd>lua require('tutor').pick_under_cursor()<CR>", { noremap = true, silent = true })
    vim.api.nvim_buf_set_keymap(buf, "n", "t", "<cmd>lua require('tutor').test_under_cursor()<CR>", { noremap = true, silent = true })
    vim.api.nvim_buf_set_keymap(buf, "n", "r", "<cmd>lua require('tutor').dashboard()<CR>", { noremap = true, silent = true })
    vim.api.nvim_buf_set_keymap(buf, "n", "q", "<cmd>close<CR>", { noremap = true, silent = true })
end

function M.get_prob_id_under_cursor()
    local line = vim.api.nvim_get_current_line()
    local id = line:match("(%d%d%d%-[%w%-_]+)")
    return id
end

function M.pick_under_cursor()
    local id = M.get_prob_id_under_cursor()
    if not id then
        vim.notify("No problem ID detected on current line", vim.log.levels.WARN)
        return
    end
    M.pick(id)
end

function M.test_under_cursor()
    local id = M.get_prob_id_under_cursor()
    M.test(id)
end

function M.pick(prob_id)
    local bin = get_tutor_bin()
    -- Run pick --no-edit to scaffold files and output path
    local res = vim.fn.system(bin .. " pick " .. prob_id .. " --no-edit")
    local solution_path = res:match("Solution%s*:%s*([%S]+)")
    if solution_path and vim.fn.filereadable(solution_path) == 1 then
        vim.cmd("edit " .. vim.fn.fnameescape(solution_path))
        vim.notify("Opened solution: " .. solution_path, vim.log.levels.INFO)
    else
        run_in_float(bin .. " pick " .. prob_id, "Tutor Pick: " .. prob_id)
    end
end

function M.test(prob_id)
    local bin = get_tutor_bin()
    local cmd = bin .. " test " .. (prob_id or "")
    run_in_float(cmd, "Tutor Test " .. (prob_id or ""))
end

function M.pull(remote)
    local bin = get_tutor_bin()
    local cmd = bin .. " pull " .. (remote or "")
    run_in_float(cmd, "Tutor Pull")
end

function M.setup()
    vim.api.nvim_create_user_command("Tutor", function(opts)
        local args = vim.split(opts.args, "%s+", { trimempty = true })
        local sub = args[1] or "list"

        if sub == "list" or sub == "ls" then
            M.dashboard()
        elseif sub == "pick" and args[2] then
            M.pick(args[2])
        elseif sub == "test" then
            M.test(args[2])
        elseif sub == "pull" or sub == "sync" then
            M.pull(args[2])
        else
            M.dashboard()
        end
    end, {
        nargs = "*",
        complete = function(_, line)
            local parts = vim.split(line, "%s+", { trimempty = true })
            if #parts <= 2 then
                return { "list", "pick", "test", "pull", "status" }
            end
            return {}
        end,
    })
end

-- Auto-register user command on require
M.setup()

return M
