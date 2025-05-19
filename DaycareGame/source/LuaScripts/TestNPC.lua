dialog = {
	{ text = "hi" },
	{ text = "whatchu thinking?", choices = {
			{ text = "Yes", next = 3 },
			{ text = "No", next = 4 }
		}
	},
	{ text = "impressive", quit = true },
	{ text = "ok" },
	{ text = "socrates", quit = true },
	index = 0
}

function interact(input)
	get_next_line(dialog, input)
end