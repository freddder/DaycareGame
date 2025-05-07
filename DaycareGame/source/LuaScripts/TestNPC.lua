dialog = {
	{ text = "hi" },
	{ text = "whatchu thinking?", quit = true, choices = {
			{ text = "I dont think", next = 3 },
			{ text = "Whole lotta stuff", next = 4 }
		}
	},
	{ text = "impressive", quit = true },
	{ text = "ok" },
	{ text = "socrates", quit = true },
	index = 0
}

function interact(input)
	s = get_next_line(dialog)
end